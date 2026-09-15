#pragma once

#include <string>
#include <vector>

namespace proc {

struct Result {
    int exit_code = -1;
    std::string out;
    std::string err;

    bool ok() const { return exit_code == 0; }
};

// Run a program (looked up on PATH) without a shell, optionally feeding it
// `input` on stdin, and capture its stdout/stderr.
// Throws std::runtime_error if the process cannot be spawned.
Result capture(const std::vector<std::string>& args, const std::string& input = {});

// Run a program inheriting the parent's stdin/stdout/stderr, for commands that
// should behave interactively. Returns the child's exit code.
// Throws std::runtime_error if the process cannot be spawned.
int run(const std::vector<std::string>& args);

}  // namespace proc
