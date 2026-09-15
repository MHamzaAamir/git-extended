#pragma once

#include <string>
#include <vector>

namespace commands {

using Args = std::vector<std::string>;

int open(const Args& args);
int back(const Args& args);
int copy(const Args& args);

struct Entry {
    const char* name;
    const char* summary;
    int (*run)(const Args&);
};

// The single place new subcommands are registered.
const std::vector<Entry>& registry();

const Entry* find(const std::string& name);

void print_usage();

}  // namespace commands
