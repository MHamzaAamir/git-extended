# git-extended

A small, extensible C++17 CLI that adds quality-of-life subcommands to Git.

Everything is delivered by a single executable, `git-extended`, which inspects
the name it was invoked as and dispatches to the matching subcommand. Git's own
external-command mechanism runs any `git-<command>` executable found on `PATH`,
so installing a symlink named `git-<command>` is all it takes to make
`git <command>` available. The set of subcommands is data-driven, so new ones
can be added without touching the dispatch logic; the current set is listed
under [Usage](#usage) below.

## Platform support

This tool currently targets **macOS only** (it relies on native macOS utilities
for clipboard and browser integration). Support for **Windows and Linux may roll
out in the future**; the platform-specific pieces are isolated in
`src/platform/`, with per-command helpers kept separate from the shared logic, to
make that straightforward.

## Requirements

- A C++17 compiler (AppleClang is fine)
- CMake 3.16+
- Git

No third-party libraries.

## Build

```bash
cmake -S . -B build
cmake --build build
```

This produces `build/git-extended`.

## Install

```bash
cmake --install build
```

By default the prefix is `/usr/local`, so the binary and symlinks land in
`/usr/local/bin`. To choose another prefix:
```bash
cmake --install build --prefix "$HOME/.local"
```

Make sure the bin directory is on your `PATH`, e.g. for the default prefix:

```bash
export PATH="/usr/local/bin:$PATH"
```

The install step places `git-extended` in the prefix's `bin` directory and
creates three symlinks next to it:

```
git-open -> git-extended
git-back -> git-extended
git-copy -> git-extended
```

Verify the install:

```bash
git-extended --version   # git-extended 1.0.0
ls -l /usr/local/bin/git-*   # or your custom prefix's bin directory
```

## Usage

### `git copy`

Copies the current branch name to the system clipboard.

```bash
$ git copy
copied: feature/CU-1832-Header-fix-unit-type-sticky
```

If `HEAD` is detached, or you are not inside a repository, it prints an error
and exits non-zero instead of copying anything misleading.

### `git back`

A convenience wrapper that delegates to Git:

```bash
git switch -
```

It never reimplements the switch logic, so behavior and output match Git
exactly.

### `git open`

Opens the current repository's remote in the default browser. It defaults to the
`origin` remote; pass another remote name to override.

```bash
git open            # opens origin
git open upstream   # opens the "upstream" remote
```

Common GitHub/GitLab-style remote URLs are normalized before opening:

| Remote                                   | Opened                                   |
| ---------------------------------------- | ---------------------------------------- |
| `git@github.com:user/repo.git`           | `https://github.com/user/repo`           |
| `https://github.com/user/repo.git`       | `https://github.com/user/repo`           |
| `ssh://git@gitlab.com/group/sub/repo.git`| `https://gitlab.com/group/sub/repo`      |
| `git://github.com/user/repo.git`         | `https://github.com/user/repo`           |

## Project layout

```
src/
  main.cpp              # argv[0] dispatch + top-level help/version
  commands/
    commands.h          # command registry + declarations
    commands.cpp        # registry, lookup, usage text
    open.cpp            # git open
    back.cpp            # git back
    copy.cpp            # git copy
  git/
    git.h
    git.cpp             # thin wrappers over the git CLI + URL normalization
  platform/
    process.h
    process.cpp         # fork/exec capture + inherit (no system())
    clipboard.h
    clipboard.cpp       # pbcopy on macOS
```

## Adding another `git-<command>`

1. Add a handler `int mycommand(const commands::Args& args);` in a new
   `src/commands/mycommand.cpp`. Return an exit code; print errors to `stderr`.
   Declare it in `src/commands/commands.h`.
2. Register it in `commands::registry()` in `src/commands/commands.cpp`:

   ```cpp
   {"mycommand", "short description shown in help", mycommand},
   ```

3. Add `src/commands/mycommand.cpp` to the `git_extended_core` sources in
   `CMakeLists.txt`, and add `mycommand` to the `IN ITEMS` list in the symlink
   `foreach` loop.
4. Rebuild and install:

   ```bash
   cmake --build build && cmake --install build
   ```

Nothing in `main.cpp` needs to change — dispatch is entirely driven by the
registry and `argv[0]`.
