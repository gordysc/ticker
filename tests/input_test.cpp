// Tests for InputLatch: each key press reaches exactly one tick, however
// the frames and ticks line up.

#include "input.h"

#include <vector>

#include "check.h"

namespace ticker {
namespace {

/// @brief Makes an input with only the jump press set.
/// @return The input.
Input JumpPress() {
  Input input;
  input.jump_pressed = true;
  return input;
}

/// @brief A press on a frame with no tick reaches the next tick.
void TestPressWaitsForATick() {
  InputLatch latch;
  latch.AddFrame(JumpPress());  // A fast frame: no tick runs.
  latch.AddFrame(Input{});      // The next frame: the key is no longer new.
  CHECK(latch.TakeForTick().jump_pressed);
}

/// @brief A press on a frame with several ticks reaches only the first.
void TestPressReachesOneTick() {
  InputLatch latch;
  latch.AddFrame(JumpPress());
  CHECK(latch.TakeForTick().jump_pressed);
  CHECK(!latch.TakeForTick().jump_pressed);
  CHECK(!latch.TakeForTick().jump_pressed);
}

/// @brief Held keys follow the latest frame and are not used up.
void TestHeldKeysFollowLatestFrame() {
  InputLatch latch;
  Input right;
  right.right = true;
  latch.AddFrame(right);
  CHECK(latch.TakeForTick().right);
  CHECK(latch.TakeForTick().right);
  latch.AddFrame(Input{});
  CHECK(!latch.TakeForTick().right);
}

}  // namespace
}  // namespace ticker

/// @brief Runs every input test.
/// @return 0 if all checks pass, otherwise 1.
int main() {
  using namespace ticker;
  const std::vector<testing::Test> tests = {
      {"press waits for a tick", TestPressWaitsForATick},
      {"press reaches one tick", TestPressReachesOneTick},
      {"held keys follow latest frame", TestHeldKeysFollowLatestFrame},
  };
  return testing::RunTests(tests);
}
