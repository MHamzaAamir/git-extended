#include "commands/commands.h"

#include "git/git.h"
#include "platform/process.h"

#include <iostream>

namespace commands {

int open(const Args& args) {
    if (args.size() > 1) {
        std::cerr << "git open: usage: git open [remote]\n";
        return 2;
    }
    const std::string remote = args.empty() ? "origin" : args.front();

    const auto url = git::remote_url(remote);
    if (!url) {
        std::cerr << "git open: remote '" << remote << "' not found (see 'git remote -v')\n";
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
