#include "commands/commands.h"

#include "git/git.h"
#include "platform/clipboard.h"

#include <iostream>

namespace commands {

const std::vector<Entry>& registry() {
    static const std::vector<Entry> entries = {
        {"open", "open the repository's remote URL in the browser", open},
        {"back", "switch back to the previous branch (git switch -)", back},
        {"copy", "copy the current branch name to the clipboard", copy},
        {"pcopy", "partial copy: branch name without its first '/'-segment", pcopy},
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
    std::cout << "\ninstalled as symlinks: git-open, git-back, git-copy, git-pcopy\n";
}

std::optional<std::string> current_branch_or_report(const std::string& command) {
    const auto branch = git::current_branch();
    if (branch) {
        return branch;
    }
    if (git::is_detached_head()) {
        std::cerr << command << ": HEAD is detached; there is no branch name to copy\n";
    } else {
        std::cerr << command << ": not inside a git repository\n";
    }
    return std::nullopt;
}

int copy_text_or_report(const std::string& command, const std::string& text) {
    std::string error;
    if (!platform::copy_to_clipboard(text, error)) {
        std::cerr << command << ": " << error << "\n";
        return 1;
    }
    std::cout << "copied: " << text << "\n";
    return 0;
}

}  // namespace commands
