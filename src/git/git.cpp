#include "git/git.h"

#include "platform/process.h"

namespace {

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

}  // namespace

namespace git {

bool is_inside_repo() {
    proc::Result result = proc::capture({"git", "rev-parse", "--is-inside-work-tree"});
    return result.ok() && trim(result.out) == "true";
}

bool is_detached_head() {
    if (!is_inside_repo()) {
        return false;
    }
    proc::Result result = proc::capture({"git", "symbolic-ref", "--quiet", "HEAD"});
    return !result.ok();
}

std::optional<std::string> current_branch() {
    proc::Result result = proc::capture({"git", "symbolic-ref", "--quiet", "--short", "HEAD"});
    if (!result.ok()) {
        return std::nullopt;
    }
    std::string branch = trim(result.out);
    if (branch.empty()) {
        return std::nullopt;
    }
    return branch;
}

std::optional<std::string> remote_url(const std::string& remote) {
    proc::Result result = proc::capture({"git", "remote", "get-url", remote});
    if (!result.ok()) {
        return std::nullopt;
    }
    std::string url = trim(result.out);
    if (url.empty()) {
        return std::nullopt;
    }
    return url;
}

std::optional<std::string> to_web_url(const std::string& remote_url) {
    const std::string url = trim(remote_url);
    if (url.empty()) {
        return std::nullopt;
    }

    std::string host;
    std::string path;

    const auto scheme = url.find("://");
    if (scheme != std::string::npos) {
        // e.g. https://github.com/user/repo.git or ssh://git@gitlab.com/group/repo.git
        const std::string rest = url.substr(scheme + 3);
        const auto slash = rest.find('/');
        if (slash == std::string::npos) {
            return std::nullopt;
        }
        host = rest.substr(0, slash);
        path = rest.substr(slash + 1);
        const auto at = host.find('@');
        if (at != std::string::npos) {
            host = host.substr(at + 1);
        }
        const auto colon = host.find(':');
        if (colon != std::string::npos) {
            host = host.substr(0, colon);
        }
    } else {
        // scp-like syntax, e.g. git@github.com:user/repo.git
        const auto colon = url.find(':');
        if (colon == std::string::npos) {
            return std::nullopt;
        }
        host = url.substr(0, colon);
        path = url.substr(colon + 1);
        const auto at = host.find('@');
        if (at != std::string::npos) {
            host = host.substr(at + 1);
        }
    }

    while (!path.empty() && path.front() == '/') {
        path.erase(path.begin());
    }
    if (host.empty() || path.empty()) {
        return std::nullopt;
    }
    if (path.size() > 4 && path.compare(path.size() - 4, 4, ".git") == 0) {
        path.resize(path.size() - 4);
    }

    return "https://" + host + "/" + path;
}

int switch_previous() {
    return proc::run({"git", "switch", "-"});
}

}  // namespace git
