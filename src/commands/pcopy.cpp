#include "commands/commands.h"

#include <iostream>
#include <string>

namespace {

// Drops the first '/'-separated segment, e.g. "feat/random-branch" -> "random-branch".
// Only the first slash is considered; later slashes are left untouched. If there
// is no slash, the whole branch name is returned.
std::string without_first_segment(const std::string& branch) {
    const auto slash = branch.find('/');
    if (slash == std::string::npos) {
        return branch;
    }
    return branch.substr(slash + 1);
}

}  // namespace

namespace commands {

int pcopy(const Args& args) {
    if (!args.empty()) {
        std::cerr << "git pcopy: usage: git pcopy\n";
        return 2;
    }

    const auto branch = current_branch_or_report("git pcopy");
    if (!branch) {
        return 1;
    }

    return copy_text_or_report("git pcopy", without_first_segment(*branch));
}

}  // namespace commands
