# The game loop in Ticker

This guide explains the game loop and the fixed timestep. It connects each
idea to the code that shows it.

## The stages

The git history builds the game loop in stages. Each stage has a tag.

| Tag | What changes |
|---|---|
| `stage-1-variable-timestep` | Each frame advances the world by the measured frame time. |
| `stage-2-fixed-step-per-frame` | Each frame advances the world by one fixed tick of 1/60 s. |

To see a stage, check out its tag:
```bash
git checkout stage-1-variable-timestep
```

To see the files that one stage changes, compare two tags:
```bash
git diff --stat stage-1-variable-timestep stage-2-fixed-step-per-frame
```

To go back to the latest code, check out `main`:
```bash
git checkout main
```

## The parts of a game loop

Each pass of the loop is a frame. A frame does three things:
1. It reads the input.
2. It updates the simulation.
3. It draws the result.

In Ticker, `App::Frame()` in `src/app.cpp` is one frame. `Step()` in
`src/world.cpp` is the update. `Step()` reads no clock and no keyboard. The
caller gives it the input and the time to advance.

From stage 2, the M key changes the loop mode while the game runs. The
sidebar shows the mode. Use M to compare a stage with the stages before it.

This split is the most important design decision in the project. Every
stage changes how `App` calls `Step()`. No stage changes what `Step()`
does with the time it gets.

The `World` struct holds all of the simulation state. It is a plain struct
of values with no pointers. A copy of a `World` is a full snapshot of the
game. Stage 4 and stage 5 use this.

## Stage 1: the variable timestep

`App::Frame()` measures the time since the start of the last frame. It
passes that time to `Step()` as `dt`. Each speed in `Step()` is multiplied
by `dt`, so the player runs the same distance each second at any frame
rate. Shardfield uses the same loop.

This loop is simple, and it looks correct. The problems come from the fact
that the size of each step changes from frame to frame.

### Try it

1. Start the game.
2. Stand next to the block in the bottom-left corner and jump. The sidebar
   shows the jump height. The player can get onto the block.
3. Push 1 to set the frame rate cap to 15 FPS.
4. Jump next to the block again. The jump is now too low.
5. Push 3 to set the frame rate cap back to 60 FPS.
6. Stand on the floor and push H. The player falls through the floor.
7. Push J to turn on the stutter. Jump from the high platforms. Sometimes
   the player falls through a platform.

### Problem 1: the result depends on the step size

`Step()` changes the velocity first, then moves the player by the new
velocity. This method is "semi-implicit Euler" integration. It assumes that
the velocity stays the same for the full step. In a real jump, gravity
changes the velocity all the time. Each step therefore has a small error,
and a bigger step has a bigger error.

The exact height of the jump is `v * v / (2 * g)`, which is 108.9 px. The
table shows the height that `Step()` gives:

| Frame rate | `dt` | Jump height |
|---|---|---|
| 144 FPS | 6.9 ms | 106.6 px |
| 60 FPS | 16.7 ms | 103.4 px |
| 30 FPS | 33.3 ms | 98.0 px |
| 15 FPS | 66.7 ms | 87.1 px |

The block is 96 px tall. A player at 15 FPS cannot get onto it. A player
on a fast computer jumps higher than a player on a slow computer. In a
game with a leaderboard or a network connection, this is a bug.

A better integration method makes the error smaller, but it does not
remove it. Physics with forces that change (for example, springs, drag or
collisions) always gives results that depend on the step size.

### Problem 2: tunneling

`MoveX()` and `MoveY()` move the player box the full distance, then look
for solid tiles at the new position. They do not look at the tiles between
the old position and the new position.

The H key freezes the program for 250 ms. The next frame passes
`dt = 0.25` to `Step()`. Gravity adds 500 px/s to the velocity, and the
player then moves 125 px in one step. The floor is 32 px thick, so the box
goes from above the floor to below it. The box never overlaps the floor,
so the collision test finds nothing. This is "tunneling".

A swept collision test can fix tunneling. It checks the full path of the
box. But it does not fix problem 1.

### Problem 3: no two runs are the same

The frame times come from the computer, not from the game. They change
with the load on the computer, other programs and the operating system.
The same key presses therefore give a slightly different game each time.
Stage 5 shows why that matters for replays.

### Code to read

- `App::Frame()` in `src/app.cpp`: the variable timestep loop.
- `Step()`, `MoveX()` and `MoveY()` in `src/world.cpp`.
- `TestJumpHeightDependsOnStepSize()` and
  `TestBigStepTunnelsThroughFloor()` in `tests/world_test.cpp`.

## Stage 2: a fixed step per frame

The second loop mode ignores the frame time. Each frame calls `Step()` one
time with `dt = kTickSeconds`, which is 1/60 s. One call to `Step()` with
this fixed `dt` is a "tick".

### What this fixes

Every step now has the same size, so the physics gives the same result at
every frame rate. The jump rises 103.4 px at 15 FPS and at 144 FPS.

The fixed tick also fixes tunneling. The fastest speed in the game is the
maximum fall speed, 900 px/s. In one tick the player falls at most 15 px,
which is less than the 32 px of a tile. Three `static_assert` lines at the
top of `src/world.cpp` check this at compile time. If a change to
`src/tuning.h` breaks the rule, the build fails.

### What goes wrong

The game now runs at the speed of the frames. At 15 FPS the world gets 15
ticks each second, so a quarter of a second of game time passes in each
real second. At 144 FPS the game runs 2.4 times too fast. The "game speed"
line in the sidebar shows the ratio of game time to real time.

Many old console games used this loop. The console and the TV always ran
at the same rate, so the loop worked. A European TV ran at 50 Hz instead of
60 Hz, and those games ran about 17% slower in Europe.

### Try it

1. Push M until the sidebar shows "2. fixed step per frame".
2. Jump next to the block. The jump height is 103.4 px.
3. Push 1 to set 15 FPS. Jump again. The height is still 103.4 px, but the
   game runs in slow motion. The game speed is about 0.25.
4. Push 5 to remove the frame rate cap. The game speed goes far above 1.
5. Push 3, then push H. The player does not fall through the floor.

### Code to read

- `App::Update()` in `src/app.cpp`: the two loop modes side by side.
- `kTickSeconds` in `src/tuning.h`.
- The `static_assert` lines at the top of `src/world.cpp`.
