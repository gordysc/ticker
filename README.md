# Ticker

Ticker is a small 2D platformer in C++20 with
[raylib](https://www.raylib.com/).

## Build in CLion
1. Open this folder in CLion.
2. Wait for CMake to download raylib. This step needs an internet connection.
3. Select the `ticker` run configuration.
4. Press Run.

## Build from the command line
```bash
cmake -S . -B build
cmake --build build
./build/ticker
```

## Controls
- Left and Right arrows: run.
- Space or Up arrow: jump.
- Esc: close the game.
