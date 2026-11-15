cmake_minimum_required(VERSION 3.24)
project(ticker LANGUAGES C CXX)

# Compile as C++20, and fail rather than quietly fall back to an older
# standard if the compiler doesn't support it.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Write compile_commands.json into the build folder. Tools like clangd and
# clang-tidy read it to learn how each file is compiled.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch raylib at configure time (no system install needed).
include(FetchContent)
# Skip raylib's example programs and games; we only need the library.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
# raylib's CMakeLists.txt asks for a minimum CMake version below 3.10, which
# makes newer CMake print a deprecation warning meant for raylib's own
# developers. CMAKE_POLICY_VERSION_MINIMUM (CMake 4.0+) tells CMake to treat
# that request as 3.10 instead, so the warning goes away. It is set only
# while raylib is set up, then removed so it doesn't affect anything else.
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
FetchContent_MakeAvailable(raylib)
unset(CMAKE_POLICY_VERSION_MINIMUM)

# Build raylib's own source files with compiler warnings off (-w). They are
# third-party code we don't change, so the build log only shows warnings
# from this project's code.
target_compile_options(raylib PRIVATE -w)

# The game program. Only .cpp files are listed: each one is compiled on its
# own, and headers are compiled as part of every .cpp file that includes them.
add_executable(ticker
    src/main.cpp
)

# Link raylib into the game. Linking a CMake target like `raylib` also adds
# its include folders, which is how `#include <raylib.h>` is found.
target_link_libraries(ticker PRIVATE raylib)

# -Wall and -Wextra turn on most useful compiler warnings. -Wpedantic warns
# about compiler-specific extensions that aren't standard C++.
target_compile_options(ticker PRIVATE -Wall -Wextra -Wpedantic)

# macOS system frameworks raylib needs: windows (Cocoa), keyboard and
# controller input (IOKit), graphics (OpenGL) and frame timing (CoreVideo).
if (APPLE)
    target_link_libraries(ticker PRIVATE
        "-framework Cocoa" "-framework IOKit" "-framework OpenGL" "-framework CoreVideo")
endif()
