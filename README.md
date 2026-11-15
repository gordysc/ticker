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

## Run the tests
Build the project first, then run CTest:
```bash
ctest --test-dir build --output-on-failure
```
The tests do not open a window.

## Controls
- Left and Right arrows: run.
- Space or Up arrow: jump.
- Esc: close the game.

### Frame rate controls
- 1, 2, 3, 4, 5: cap the frame rate at 15, 30, 60 or 144 FPS, or remove the cap.
- H: freeze the program for 250 ms one time (a "hitch").
- J: turn random short freezes ("stutter") on or off.
