# Git-extended

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

## Distribution

There are currently **no prebuilt binaries and no package-manager packages**
(no Homebrew formula, MacPorts port, and so on). The only supported way to get
`git-extended` (assuming you really want it) is to [build it from source](#build) with CMake, which
takes only a few seconds and has no third-party dependencies.

Prebuilt releases and package-manager distribution may be added in the future.

## Build

First clone the repository:

```bash
git clone https://github.com/MHamzaAamir/git-extended.git
cd git-extended
```

Then configure and build:

```bash
cmake -S . -B build
cmake --build build
```

All generated files go in `build/`, including the executable at
`build/git-extended`.

## Install

Installing copies the `git-extended` binary and the `git-<command>` symlinks
into a `bin` directory. CMake picks that directory from an install **prefix**:
`<prefix>/bin`. There are two common choices.

### Global install (system-wide)

Uses the default prefix `/usr/local`, so everything goes to `/usr/local/bin`:

```bash
sudo cmake --install build
```

`/usr/local` is owned by `root`, so this typically needs `sudo`. The result is available
to every user account on the machine.

### Local install (per-user)

Uses a prefix inside your home directory, so everything goes to
`~/.local/bin`:

```bash
cmake --install build --prefix "$HOME/.local"
```

No `sudo` is needed, nothing outside your home directory is touched, and only
your account sees the commands. This is the recommended option for personal
use.

You can install to any other prefix with the same flag, e.g.
`--prefix "$HOME/tools"` puts things in `~/tools/bin`.

### Why you may need to update `PATH`

Installing the files is not enough for the shell to run them by name. `PATH` is
the list of directories your shell searches when you type a command. You can
print it with:

```bash
echo "$PATH"
```

If the install's `bin` directory is not in that list, running `git-extended`
(or `git open`) fails with `command not found`. Concretely:

- `/usr/local/bin` is already on `PATH` on macOS, so a **global install works
  immediately**.
- `~/.local/bin` is usually **not** on `PATH` by default, so a **local install
  needs one extra step**. Add it to your shell config — `~/.zshrc` on macOS
  (zsh) or `~/.bashrc` for bash — so it takes effect in every new terminal:

  ```bash
  export PATH="$HOME/.local/bin:$PATH"
  ```

  Then reload it with `source ~/.zshrc` (or just open a new terminal).

  Prepending (`"$HOME/.local/bin:$PATH"`) puts your local install *ahead of* any
  system-wide copies. To confirm which copy your shell will run:

  ```bash
  which git-extended
  ```

### What gets installed

Either way, the prefix's `bin` directory ends up with the binary plus three
symlinks, which is what lets `git open`, `git back`, and `git copy` work:

```
git-extended
git-open  -> git-extended
git-back  -> git-extended
git-copy  -> git-extended
```

Verify:

```bash
git-extended --version
```

Only reach for `sudo` when you actually want a system-wide install; building and
installing as your own user avoids permission surprises.

## Uninstall

Remove the files from the `bin` directory of the prefix you installed to —
`/usr/local/bin` for a global install, `~/.local/bin` for a local one:

```bash
# global
sudo rm -f /usr/local/bin/git-extended \
           /usr/local/bin/git-open \
           /usr/local/bin/git-back \
           /usr/local/bin/git-copy
```

```bash
# local
rm -f "$HOME/.local/bin/git-extended" \
      "$HOME/.local/bin/git-open" \
      "$HOME/.local/bin/git-back" \
      "$HOME/.local/bin/git-copy"
```

You can then delete the `build/` directory.

## Usage

### `git copy`

Copies the current branch name to the system clipboard.

```bash
$ git copy
copied: feature/CU-1832-Header-fix-unit-type-sticky
```

Pass `-p` (or `--partial`) for a **partial copy**: the first `/`-separated
segment is dropped before copying. This is handy when branches are prefixed by a
type or ticket, e.g. `feat/random-branch-name` → `random-branch-name`.

```bash
$ git copy -p
copied: random-branch-name
```

Only the **first** slash is used as the split point, so any later slashes stay
intact:

| Branch                    | `git copy -p` copies |
| ------------------------- | -------------------- |
| `feat/random-branch-name` | `random-branch-name` |
| `feat/a/b`                | `a/b`                |
| `feature/CU-1832/fix`     | `CU-1832/fix`        |
| `main` (no slash)         | `main`               |

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

Opens the current repository's `origin` remote in the default browser.

```bash
git open   # opens the URL configured for the origin remote
```

Common GitHub/GitLab-style remote URLs are normalized before opening:

| Remote                                   | Opened                                   |
| ---------------------------------------- | ---------------------------------------- |
| `git@github.com:user/repo.git`           | `https://github.com/user/repo`           |
| `https://github.com/user/repo.git`       | `https://github.com/user/repo`           |
| `ssh://git@gitlab.com/group/sub/repo.git`| `https://gitlab.com/group/sub/repo`      |
| `git://github.com/user/repo.git`         | `https://github.com/user/repo`           |


## Note

This is a personal project that grows organically. I add new subcommands as they
become useful to me or as I hit a need for them.