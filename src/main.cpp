#include "commands/commands.h"

#include <csignal>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#ifndef GIT_EXTENDED_VERSION
#define GIT_EXTENDED_VERSION "dev"
#endif

namespace {

std::string basename_of(const std::string& path) {
    const auto slash = path.find_last_of('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

}  // namespace

int main(int argc, char** argv) {
    // A child (e.g. pbcopy) closing its stdin must not kill us with SIGPIPE.
    std::signal(SIGPIPE, SIG_IGN);

    std::vector<std::string> args(argv, argv + argc);
    if (args.empty()) {
        args.push_back("git-extended");
    }

    // Git runs external subcommands as `git-<name>`, so argv[0] tells us the
    // command when invoked through a symlink. Otherwise fall back to argv[1].
    const std::string invoked = basename_of(args[0]);

    std::string command;
    commands::Args rest;

    if (invoked.rfind("git-", 0) == 0 && invoked != "git-extended") {
        command = invoked.substr(4);
        rest.assign(args.begin() + 1, args.end());
    } else {
        if (args.size() < 2) {
            commands::print_usage();
            return 1;
        }
        command = args[1];
        rest.assign(args.begin() + 2, args.end());
    }

    if (command == "help" || command == "--help" || command == "-h") {
        commands::print_usage();
        return 0;
    }
    if (command == "version" || command == "--version" || command == "-v") {
        std::cout << "git-extended " << GIT_EXTENDED_VERSION << "\n";
        return 0;
    }

    const commands::Entry* entry = commands::find(command);
    if (entry == nullptr) {
        std::cerr << "git-extended: unknown command '" << command << "'\n\n";
        commands::print_usage();
        return 1;
    }

    try {
        return entry->run(rest);
    } catch (const std::exception& e) {
        std::cerr << "git-extended: " << e.what() << "\n";
        return 1;
    }
}
