# Ticker

Ticker is a small 2D platformer in C++20 with
[raylib](https://www.raylib.com/).

The game is also an introduction to the game loop and the fixed timestep.
The git history builds the loop in stages. Read
[docs/game-loop.md](docs/game-loop.md) for the guide.

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

### Debug controls
The sidebar to the right of the level shows the state of the game loop.
- M: change to the next loop mode. The sidebar shows the mode.
- 1, 2, 3, 4, 5: cap the frame rate at 15, 30, 60 or 144 FPS, or remove the cap.
- H: freeze the program for 250 ms one time (a "hitch").
- J: turn random short freezes ("stutter") on or off.
- K: make each tick take 25 ms of real time, or go back to normal.
- S: turn slow motion (0.1 times real speed) on or off.
- F: hold for fast forward (4 times real speed).
- G: show or hide outlines of the player at the last two ticks.
- P: pause the game, or continue a paused game.
- N: while paused, advance the game by one tick.
- R: start a recording. Push R again to stop it and play it back. Push R
  during the playback to stop it.
