#pragma once

#include <optional>
#include <string>

namespace git {

// True when the current working directory is inside a git work tree.
bool is_inside_repo();

// True when HEAD is detached (only meaningful inside a repository).
bool is_detached_head();

// Name of the currently checked-out branch, or nullopt when detached/outside a repo.
std::optional<std::string> current_branch();

// URL configured for `remote`, or nullopt when it does not exist.
std::optional<std::string> remote_url(const std::string& remote = "origin");

// Turn a git remote URL (SSH or HTTPS) into a browsable web URL.
std::optional<std::string> to_web_url(const std::string& remote_url);

// Replace this process with `git switch -` and return its exit code.
int switch_previous();

}  // namespace git
