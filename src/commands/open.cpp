#include "commands/commands.h"

#include "git/git.h"
#include "platform/process.h"

#include <iostream>

namespace commands {

int open(const Args& args) {
    if (!args.empty()) {
        std::cerr << "git open: usage: git open\n";
        return 2;
    }

    const auto url = git::remote_url();
    if (!url) {
        std::cerr << "git open: remote 'origin' not found (see 'git remote -v')\n";
        return 1;
    }

    const auto web_url = git::to_web_url(*url);
    if (!web_url) {
        std::cerr << "git open: cannot turn remote URL into a web URL: " << *url << "\n";
        return 1;
    }

    const auto result = proc::capture({"open", *web_url});
    if (!result.ok()) {
        std::cerr << "git open: failed to open " << *web_url << "\n";
        return 1;
    }
    return 0;
}

}  // namespace commands
