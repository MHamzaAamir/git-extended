#include "commands/commands.h"

#include "git/git.h"
#include "platform/clipboard.h"

#include <iostream>

namespace commands {

int copy(const Args& args) {
    if (!args.empty()) {
        std::cerr << "git copy: usage: git copy\n";
        return 2;
    }

    const auto branch = git::current_branch();
    if (!branch) {
        if (git::is_detached_head()) {
            std::cerr << "git copy: HEAD is detached; there is no branch name to copy\n";
        } else {
            std::cerr << "git copy: not inside a git repository\n";
        }
        return 1;
    }

    std::string error;
    if (!platform::copy_to_clipboard(*branch, error)) {
        std::cerr << "git copy: " << error << "\n";
        return 1;
    }

    std::cout << "copied: " << *branch << "\n";
    return 0;
}

}  // namespace commands
