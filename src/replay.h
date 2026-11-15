#ifndef TICKER_REPLAY_H_
#define TICKER_REPLAY_H_

// Header guard; see screen.h for how it works.

#include <cstdint>
#include <vector>

#include "input.h"
#include "level.h"
#include "world.h"

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief What the replay system is doing. The R key moves between them.
enum class ReplayState {
  /// @brief Normal play. Nothing is recorded.
  Off,
  /// @brief Each step's input goes into the replay.
  Recording,
  /// @brief The replay's inputs drive the steps. The keyboard is ignored.
  Playing,
};

/// @brief How the last playback ended.
enum class ReplayResult {
  /// @brief No playback has finished yet.
  None,
  /// @brief The world at the end had the recorded hash.
  Matched,
  /// @brief The world at the end was different from the recording.
  Desynced,
};

/// @brief A recorded game: everything needed to play it again.
///
/// A replay does not store positions or a video. It stores only what went
/// into the simulation: the seed and the input of every step. Step() is
/// deterministic, so the same seed and the same inputs give the same game,
/// bit for bit. That makes a replay very small: one Input per tick.
///
/// The catch: "the same inputs" must include the same dt for every step.
/// With a fixed tick that is always true. With a variable timestep, each
/// step's dt comes from the frame time, and playback has different frame
/// times from the recording.
struct Replay {
  /// @brief The seed the world started with.
  std::uint32_t seed = 0;
  /// @brief The input of each step, in order.
  std::vector<Input> inputs;
  /// @brief HashWorld() of the world after the last step.
  std::uint64_t final_hash = 0;
};

/// @brief Plays a replay from start to end with fixed ticks, all at once.
///
/// The game plays replays one tick at a time, so the player can watch. This
/// function is for tests and tools: it gives the end result directly.
///
/// @param replay The replay.
/// @param level The level it was recorded in.
/// @return The world after the last step.
World PlayBack(const Replay& replay, const Level& level);

}  // namespace ticker

#endif  // TICKER_REPLAY_H_
