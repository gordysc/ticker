// Simulation tests. They build a world, call Step() with hand-made inputs
// and step sizes, and check the result. Step() reads no clock and no
// keyboard, so no window is needed.

#include "world.h"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "check.h"
#include "input.h"
#include "level.h"
#include "random.h"
#include "tuning.h"

namespace ticker {
namespace {

/// @brief One 60 FPS frame, in seconds.
constexpr float kFrame60 = 1.0f / 60.0f;
/// @brief One 15 FPS frame, in seconds.
constexpr float kFrame15 = 1.0f / 15.0f;
/// @brief The y of the top of the floor, in pixels.
constexpr float kFloorTop = 21 * kTileSize;
/// @brief Height of the block in the bottom-left corner, in pixels.
constexpr float kWallHeight = 3 * kTileSize;

/// @brief Checks that two floats are equal to within a small tolerance.
///
/// Floating-point maths rounds at every step, so results that "should" be
/// equal often differ in the last few bits. Comparing with == would make
/// the tests fail for no real reason.
///
/// @param a First value.
/// @param b Second value.
/// @return True if they differ by less than 0.01.
bool Near(float a, float b) { return std::fabs(a - b) < 0.01f; }

/// @brief The y of the player's feet.
/// @param world The world.
/// @return The bottom edge of the player's box, in pixels.
float Feet(const World& world) {
  return world.player.position.y + kPlayerHeight;
}

/// @brief Puts the player at a place, standing still.
/// @param world The world.
/// @param feet The middle of the bottom edge of the box, in pixels.
void PlacePlayer(World& world, Vector2 feet) {
  world.player = Player{};
  world.player.position = {feet.x - kPlayerWidth / 2, feet.y - kPlayerHeight};
}

/// @brief Runs Step() a number of times with the same input and step size.
/// @param world The world.
/// @param level The level.
/// @param input The input for every step.
/// @param dt The step size.
/// @param steps How many steps.
void Run(World& world, const Level& level, const Input& input, float dt,
         int steps) {
  for (int i = 0; i < steps; ++i) {
    Step(world, level, input, dt);
  }
}

/// @brief Measures how high one jump rises with a given step size.
/// @param dt The step size for every step.
/// @return The highest the feet got, in pixels above the floor.
float JumpHeight(float dt) {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  // Let the player land and settle first.
  Run(world, level, Input{}, dt, 30);
  const float start = Feet(world);

  Input jump;
  jump.jump_pressed = true;
  Step(world, level, jump, dt);

  float highest = Feet(world);
  while (world.player.velocity.y < 0) {
    Step(world, level, Input{}, dt);
    // Up is negative y, so the highest point is the smallest y.
    highest = std::min(highest, Feet(world));
  }
  return start - highest;
}

/// @brief The level map gives the expected size and marked places.
void TestLevelLoads() {
  const Level level(kLevelMap);
  CHECK(level.columns() == 30);
  CHECK(level.rows() == 22);
  CHECK(level.IsSolid(0, 0));
  CHECK(!level.IsSolid(5, 3));
  // Outside the map counts as empty.
  CHECK(!level.IsSolid(-1, 21));
  CHECK(level.coin_spawns().size() == 9);
  CHECK(Near(level.player_start().y, kFloorTop));
}

/// @brief A falling player lands on the floor and stays on it.
void TestPlayerLandsOnFloor() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  world.player.position.y -= 100;
  Run(world, level, Input{}, kFrame60, 60);
  CHECK(world.player.on_ground);
  CHECK(Near(Feet(world), kFloorTop));
  CHECK(world.falls == 0);
}

/// @brief A wall stops the player, with no overlap.
void TestWallStopsRunning() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  Input left;
  left.left = true;
  Run(world, level, left, kFrame60, 60);
  // The block in the corner covers columns 1 to 3, so its right side is at
  // the start of column 4.
  CHECK(Near(world.player.position.x, 4 * kTileSize));
}

/// @brief The jump height depends on the step size.
///
/// This is the main problem with a variable timestep. Each step is an
/// estimate of the real curve, and bigger steps give a worse estimate.
void TestJumpHeightDependsOnStepSize() {
  const float at_60 = JumpHeight(kFrame60);
  const float at_15 = JumpHeight(kFrame15);
  // At 60 FPS the jump clears the 3-tile block. At 15 FPS it does not.
  CHECK(at_60 > kWallHeight);
  CHECK(at_15 < kWallHeight);
  // Both are below the exact height of v * v / (2 * g).
  const float exact = kJumpSpeed * kJumpSpeed / (2 * kGravity);
  CHECK(at_60 < exact);
}

/// @brief The world's own jump measurement matches the test's measurement.
void TestJumpStatsRecordHeight() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  Run(world, level, Input{}, kFrame60, 30);
  Input jump;
  jump.jump_pressed = true;
  Step(world, level, jump, kFrame60);
  CHECK(world.jump.rising);
  Run(world, level, Input{}, kFrame60, 60);
  CHECK(!world.jump.rising);
  CHECK(Near(world.jump.last_height, JumpHeight(kFrame60)));
}

/// @brief One big step can carry the player through the floor.
void TestBigStepTunnelsThroughFloor() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  PlacePlayer(world, {300, kFloorTop - 10});
  world.player.velocity.y = kMaxFallSpeed;
  // A 250 ms hitch moves the player 225 px in one step. The floor is 32 px
  // thick, so the box lands below it and never overlaps it.
  Step(world, level, Input{}, 0.25f);
  CHECK(world.falls == 1);
}

/// @brief The same fall in small steps lands on the floor.
void TestSmallStepsDoNotTunnel() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  PlacePlayer(world, {300, kFloorTop - 10});
  world.player.velocity.y = kMaxFallSpeed;
  Run(world, level, Input{}, kFrame60, 15);
  CHECK(world.falls == 0);
  CHECK(world.player.on_ground);
}

/// @brief A jump pressed just after the feet leave a ledge still works.
void TestCoyoteTime() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  // Stand near the right end of the platform at row 19 (columns 9 to 12).
  PlacePlayer(world, {12 * kTileSize + 10, 19 * kTileSize});
  Run(world, level, Input{}, kFrame60, 2);
  CHECK(world.player.on_ground);

  Input right;
  right.right = true;
  while (world.player.on_ground) {
    Step(world, level, right, kFrame60);
  }
  // Off the edge now. Wait two more frames, then press jump.
  Run(world, level, right, kFrame60, 2);
  Input jump;
  jump.jump_pressed = true;
  Step(world, level, jump, kFrame60);
  CHECK(world.player.velocity.y < 0);
}

/// @brief A jump pressed just before landing happens on landing.
void TestJumpBuffer() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  // A little above the floor, at a place with nothing overhead. Column 22
  // is clear for 5 tiles above the floor.
  PlacePlayer(world, {22 * kTileSize, kFloorTop - 2});
  Input jump;
  jump.jump_pressed = true;
  Step(world, level, jump, kFrame60);
  CHECK(world.player.velocity.y >= 0);  // In the air: no jump yet.

  Run(world, level, Input{}, kFrame60, 3);
  CHECK(world.player.velocity.y < 0);  // Landed, and the jump started.
}

/// @brief Touching the coin scores and moves it somewhere else.
void TestCoinMovesWhenCollected() {
  const Level level(kLevelMap);
  World world = MakeWorld(level, 1);
  const int first = world.coin;
  const Rectangle coin = CoinBox(world, level);
  world.player.position = {coin.x, coin.y};
  Step(world, level, Input{}, kFrame60);
  CHECK(world.score == 1);
  CHECK(world.coin != first);
}

/// @brief The drawn position goes in a straight line between two ticks.
void TestLerpPlayerPosition() {
  const Level level(kLevelMap);
  World previous = MakeWorld(level, 1);
  World current = previous;
  current.player.position.x += 10;
  const Vector2 half = LerpPlayerPosition(previous, current, 0.5f);
  CHECK(Near(half.x, previous.player.position.x + 5));
  CHECK(Near(half.y, previous.player.position.y));
}

/// @brief A respawn is drawn at the new place at once, not in between.
void TestLerpSkipsRespawn() {
  const Level level(kLevelMap);
  World previous = MakeWorld(level, 1);
  previous.player.position = {500, 900};  // Far below the level.
  World current = previous;
  Step(current, level, Input{}, kFrame60);
  CHECK(current.falls == 1);
  const Vector2 drawn = LerpPlayerPosition(previous, current, 0.5f);
  CHECK(Near(drawn.x, current.player.position.x));
  CHECK(Near(drawn.y, current.player.position.y));
}

/// @brief The generator gives the same numbers for the same seed.
void TestRandomRepeats() {
  Random a(42);
  Random b(42);
  std::vector<int> from_a;
  std::vector<int> from_b;
  for (int i = 0; i < 100; ++i) {
    from_a.push_back(a.Below(9));
    from_b.push_back(b.Below(9));
  }
  CHECK(from_a == from_b);
  // std::ranges algorithms (C++20) take the whole container instead of a
  // begin() and end() pair.
  CHECK(std::ranges::all_of(from_a, [](int n) { return n >= 0 && n < 9; }));
  // A seed of 0 must not get stuck.
  Random zero(0);
  CHECK(zero.Next() != 0);
}

}  // namespace
}  // namespace ticker

/// @brief Runs every simulation test.
/// @return 0 if all checks pass, otherwise 1.
int main() {
  using namespace ticker;
  const std::vector<testing::Test> tests = {
      {"level loads", TestLevelLoads},
      {"player lands on floor", TestPlayerLandsOnFloor},
      {"wall stops running", TestWallStopsRunning},
      {"jump height depends on step size", TestJumpHeightDependsOnStepSize},
      {"jump stats record height", TestJumpStatsRecordHeight},
      {"big step tunnels through floor", TestBigStepTunnelsThroughFloor},
      {"small steps do not tunnel", TestSmallStepsDoNotTunnel},
      {"coyote time", TestCoyoteTime},
      {"jump buffer", TestJumpBuffer},
      {"coin moves when collected", TestCoinMovesWhenCollected},
      {"lerp player position", TestLerpPlayerPosition},
      {"lerp skips respawn", TestLerpSkipsRespawn},
      {"random repeats", TestRandomRepeats},
  };
  return testing::RunTests(tests);
}
