#include "commands/commands.h"

#include <iostream>

namespace commands {

const std::vector<Entry>& registry() {
    static const std::vector<Entry> entries = {
        {"open", "open the repository's remote URL in the browser", open},
        {"back", "switch back to the previous branch (git switch -)", back},
        {"copy", "copy the current branch name to the clipboard", copy},
    };
    return entries;
}

const Entry* find(const std::string& name) {
    for (const auto& entry : registry()) {
        if (name == entry.name) {
            return &entry;
        }
    }
    return nullptr;
}

void print_usage() {
    std::cout << "git-extended - quality-of-life commands for git\n\n"
              << "usage: git <command> [args]\n"
              << "   or: git-extended <command> [args]\n\n"
              << "commands:\n";
    for (const auto& entry : registry()) {
        std::cout << "  " << entry.name << "\t" << entry.summary << "\n";
    }
    std::cout << "\ninstalled as symlinks: git-open, git-back, git-copy\n";
}

}  // namespace commands
