#pragma once

#include <string>

namespace platform {

// Copy `text` to the system clipboard. On success returns true and leaves
// `error` untouched; on failure returns false and fills `error` with a message.
bool copy_to_clipboard(const std::string& text, std::string& error);

}  // namespace platform
