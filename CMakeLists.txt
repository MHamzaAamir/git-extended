cmake_minimum_required(VERSION 3.16)

project(git-extended
    VERSION 1.1.0
    DESCRIPTION "Quality-of-life commands for git"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core logic lives in a small library, keeping the entry point thin.
add_library(git_extended_core STATIC
    src/commands/commands.cpp
    src/commands/open.cpp
    src/commands/back.cpp
    src/commands/copy.cpp
    src/commands/pcopy.cpp
    src/git/git.cpp
    src/platform/process.cpp
    src/platform/clipboard.cpp)

target_include_directories(git_extended_core PUBLIC src)
target_compile_definitions(git_extended_core PUBLIC
    GIT_EXTENDED_VERSION="${PROJECT_VERSION}")

add_executable(git-extended src/main.cpp)
target_link_libraries(git-extended PRIVATE git_extended_core)

include(GNUInstallDirs)
install(TARGETS git-extended RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Git finds external subcommands as `git-<name>` executables on PATH, so install
# one symlink per command alongside the binary.
foreach(cmd IN ITEMS open back copy pcopy)
    install(CODE "
        set(_link \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/git-${cmd}\")
        file(REMOVE \"\${_link}\")
        file(CREATE_LINK \"git-extended\" \"\${_link}\" SYMBOLIC)
    ")
endforeach()
