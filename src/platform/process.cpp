#include "platform/process.h"

#include <cerrno>
#include <cstring>
#include <poll.h>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

namespace {

std::vector<char*> to_argv(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    argv.reserve(args.size() + 1);
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}

int exit_status(int status) {
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }
    return -1;
}

[[noreturn]] void fail(const std::string& what) {
    throw std::runtime_error(what + ": " + std::strerror(errno));
}

}  // namespace

namespace proc {

Result capture(const std::vector<std::string>& args, const std::string& input) {
    if (args.empty()) {
        throw std::runtime_error("cannot run an empty command");
    }

    int in_pipe[2];
    int out_pipe[2];
    int err_pipe[2];
    if (::pipe(in_pipe) != 0 || ::pipe(out_pipe) != 0 || ::pipe(err_pipe) != 0) {
        fail("pipe");
    }

    pid_t pid = ::fork();
    if (pid < 0) {
        fail("fork");
    }

    if (pid == 0) {
        ::dup2(in_pipe[0], STDIN_FILENO);
        ::dup2(out_pipe[1], STDOUT_FILENO);
        ::dup2(err_pipe[1], STDERR_FILENO);
        for (int fd : {in_pipe[0], in_pipe[1], out_pipe[0], out_pipe[1], err_pipe[0], err_pipe[1]}) {
            ::close(fd);
        }
        auto argv = to_argv(args);
        ::execvp(argv[0], argv.data());
        _exit(127);
    }

    ::close(in_pipe[0]);
    ::close(out_pipe[1]);
    ::close(err_pipe[1]);

    if (!input.empty()) {
        size_t written = 0;
        while (written < input.size()) {
            ssize_t n = ::write(in_pipe[1], input.data() + written, input.size() - written);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                break;  // e.g. EPIPE: the child is gone, stop writing
            }
            written += static_cast<size_t>(n);
        }
    }
    ::close(in_pipe[1]);

    Result result;
    struct pollfd fds[2];
    fds[0] = {out_pipe[0], POLLIN, 0};
    fds[1] = {err_pipe[0], POLLIN, 0};
    int open_fds = 2;

    while (open_fds > 0) {
        if (::poll(fds, 2, -1) < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        for (int i = 0; i < 2; ++i) {
            if (fds[i].fd < 0 || (fds[i].revents & (POLLIN | POLLHUP | POLLERR)) == 0) {
                continue;
            }
            char buffer[4096];
            ssize_t n = ::read(fds[i].fd, buffer, sizeof(buffer));
            if (n > 0) {
                std::string& sink = (i == 0) ? result.out : result.err;
                sink.append(buffer, static_cast<size_t>(n));
            } else if (n == 0 || (errno != EINTR && errno != EAGAIN)) {
                ::close(fds[i].fd);
                fds[i].fd = -1;
                --open_fds;
            }
        }
    }

    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) {
            fail("waitpid");
        }
    }

    result.exit_code = exit_status(status);
    return result;
}

int run(const std::vector<std::string>& args) {
    if (args.empty()) {
        throw std::runtime_error("cannot run an empty command");
    }

    pid_t pid = ::fork();
    if (pid < 0) {
        fail("fork");
    }

    if (pid == 0) {
        auto argv = to_argv(args);
        ::execvp(argv[0], argv.data());
        _exit(127);
    }

    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) {
            fail("waitpid");
        }
    }
    return exit_status(status);
}

}  // namespace proc
