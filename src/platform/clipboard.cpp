#include "platform/clipboard.h"

#include "platform/process.h"

namespace platform {

bool copy_to_clipboard(const std::string& text, std::string& error) {
#if defined(__APPLE__)
    proc::Result result = proc::capture({"pbcopy"}, text);
    if (!result.ok()) {
        error = "pbcopy exited with status " + std::to_string(result.exit_code);
        return false;
    }
    return true;
#else
    (void)text;
    error = "clipboard support is currently only implemented on macOS";
    return false;
#endif
}

}  // namespace platform
