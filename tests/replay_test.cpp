// Tests for determinism: the same seed and inputs must give the same world,
// however the ticks are spread over frames. The tests drive the clock and
// the simulation by hand, the same way App does, with no window.

#include "replay.h"

#include <cstdint>
#include <vector>

#include "check.h"
#include "clock.h"
#include "input.h"
#include "level.h"
#include "tuning.h"
#include "world.h"

namespace ticker {
namespace {

/// @brief The input for a given tick of a fixed test script.
///
/// Run right, jump now and then, and sometimes run left. The script is a
/// function of the tick number only, so every run of it gets the same
/// input on the same tick.
///
/// @param tick The tick number, from 0.
/// @return The input for that tick.
Input ScriptedInput(int tick) {
  Input input;
  input.right = (tick / 90) % 3 != 2;
  input.left = !input.right;
  input.jump_pressed = tick % 37 == 0;
  return input;
}

/// @brief Records a game of a number of ticks from the script.
/// @param level The level.
/// @param seed The world seed.
/// @param ticks How many ticks to record.
/// @return The replay, with the final hash filled in.
Replay Record(const Level& level, std::uint32_t seed, int ticks) {
  Replay replay;
  replay.seed = seed;
  World world = MakeWorld(level, seed);
  for (int i = 0; i < ticks; ++i) {
    const Input input = ScriptedInput(i);
    replay.inputs.push_back(input);
    Step(world, level, input, kTickSeconds);
  }
  replay.final_hash = HashWorld(world);
  return replay;
}

/// @brief Runs the script through the accumulator loop at one frame rate.
/// @param level The level.
/// @param fps Frames per second.
/// @param total_ticks How many ticks to run. The last frame stops early
///        if it reaches this number.
/// @return The final world.
World RunAtFrameRate(const Level& level, int fps, int total_ticks) {
  World world = MakeWorld(level, 7);
  FixedStepClock clock(kTickSeconds, kMaxTicksPerFrame);
  int tick = 0;
  while (tick < total_ticks) {
    const int ticks = clock.Advance(1.0 / fps);
    for (int i = 0; i < ticks && tick < total_ticks; ++i) {
      Step(world, level, ScriptedInput(tick), kTickSeconds);
      ++tick;
    }
  }
  return world;
}

/// @brief Runs the same moves with a variable timestep at one frame rate.
///
/// The input is a function of game time here, because with a variable
/// timestep there are no ticks to count.
///
/// @param level The level.
/// @param fps Frames per second.
/// @param seconds How long to run.
/// @return The final world.
World RunVariableAtFrameRate(const Level& level, int fps, int seconds) {
  World world = MakeWorld(level, 7);
  const float dt = 1.0f / fps;
  for (int frame = 0; frame < fps * seconds; ++frame) {
    // The script tick that matches this moment of game time.
    const int tick = frame * kTicksPerSecond / fps;
    Input input = ScriptedInput(tick);
    // Only press jump on the first frame of that tick, so each press is
    // used once at any frame rate.
    const int previous_tick = (frame - 1) * kTicksPerSecond / fps;
    input.jump_pressed =
        input.jump_pressed && (frame == 0 || tick != previous_tick);
    Step(world, level, input, dt);
  }
  return world;
}

/// @brief Playing a replay gives the recorded world.
void TestReplayMatches() {
  const Level level(kLevelMap);
  const Replay replay = Record(level, 1234, 1200);
  CHECK(HashWorld(PlayBack(replay, level)) == replay.final_hash);
}

/// @brief A different seed gives a different game from the same inputs.
void TestReplayNeedsTheSeed() {
  const Level level(kLevelMap);
  Replay replay = Record(level, 1234, 1200);
  // The script collects coins, and the seed decides where they appear.
  CHECK(PlayBack(replay, level).score > 0);
  replay.seed = 4321;
  CHECK(HashWorld(PlayBack(replay, level)) != replay.final_hash);
}

/// @brief With fixed ticks, the frame rate does not change the result.
void TestFixedTicksIgnoreFrameRate() {
  const Level level(kLevelMap);
  // 10 seconds of game time. At 15 FPS that is about 150 frames of 4
  // ticks, and at 144 FPS about 1440 frames of 0 or 1 tick.
  constexpr int kTicks = 600;
  const std::uint64_t at_15 = HashWorld(RunAtFrameRate(level, 15, kTicks));
  const std::uint64_t at_60 = HashWorld(RunAtFrameRate(level, 60, kTicks));
  const std::uint64_t at_144 = HashWorld(RunAtFrameRate(level, 144, kTicks));
  CHECK(at_15 == at_60);
  CHECK(at_60 == at_144);
}

/// @brief With a variable timestep, the frame rate changes the result.
void TestVariableTimestepDependsOnFrameRate() {
  const Level level(kLevelMap);
  const World at_30 = RunVariableAtFrameRate(level, 30, 10);
  const World at_60 = RunVariableAtFrameRate(level, 60, 10);
  CHECK(HashWorld(at_30) != HashWorld(at_60));
}

}  // namespace
}  // namespace ticker

/// @brief Runs every replay test.
/// @return 0 if all checks pass, otherwise 1.
int main() {
  using namespace ticker;
  const std::vector<testing::Test> tests = {
      {"replay matches", TestReplayMatches},
      {"replay needs the seed", TestReplayNeedsTheSeed},
      {"fixed ticks ignore frame rate", TestFixedTicksIgnoreFrameRate},
      {"variable timestep depends on frame rate",
       TestVariableTimestepDependsOnFrameRate},
  };
  return testing::RunTests(tests);
}
