#include "commands/commands.h"

#include "git/git.h"

#include <iostream>

namespace commands {

int back(const Args& args) {
    if (!args.empty()) {
        std::cerr << "git back: usage: git back\n";
        return 2;
    }
    // Delegate to git itself so behavior (and output) stays identical.
    return git::switch_previous();
}

}  // namespace commands
