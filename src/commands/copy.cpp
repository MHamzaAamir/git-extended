#include "commands/commands.h"

#include <iostream>

namespace commands {

int copy(const Args& args) {
    if (!args.empty()) {
        std::cerr << "git copy: usage: git copy\n";
        return 2;
    }

    const auto branch = current_branch_or_report("git copy");
    if (!branch) {
        return 1;
    }

    return copy_text_or_report("git copy", *branch);
}

}  // namespace commands
