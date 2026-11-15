#include "replay.h"

#include "tuning.h"

namespace ticker {

World PlayBack(const Replay& replay, const Level& level) {
  World world = MakeWorld(level, replay.seed);
  // `const Input&` in a range-based for loop reads each element in place.
  // A plain `Input` would copy each one first: harmless for a small struct,
  // but a reference says "only reading" more clearly.
  for (const Input& input : replay.inputs) {
    Step(world, level, input, kTickSeconds);
  }
  return world;
}

}  // namespace ticker
