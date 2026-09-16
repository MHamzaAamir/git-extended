#pragma once

#include <optional>
#include <string>
#include <vector>

namespace commands {

using Args = std::vector<std::string>;

int open(const Args& args);
int back(const Args& args);
int copy(const Args& args);
int pcopy(const Args& args);

struct Entry {
    const char* name;
    const char* summary;
    int (*run)(const Args&);
};

// The single place new subcommands are registered.
const std::vector<Entry>& registry();

const Entry* find(const std::string& name);

void print_usage();

// Helpers shared by the branch-copying commands. `command` is used to prefix
// error messages (e.g. "git copy").
std::optional<std::string> current_branch_or_report(const std::string& command);
int copy_text_or_report(const std::string& command, const std::string& text);

}  // namespace commands
