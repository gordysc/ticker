#ifndef TICKER_TUNING_H_
#define TICKER_TUNING_H_

// Header guard; see screen.h for how it works.

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief How many simulation ticks make one second.
inline constexpr int kTicksPerSecond = 60;
/// @brief The length of one tick, in seconds.
///
/// A "tick" is one call to Step() with this fixed dt. With a fixed tick,
/// the simulation gets the same step size on every computer, at every
/// frame rate.
inline constexpr float kTickSeconds = 1.0f / kTicksPerSecond;
/// @brief The most ticks that one frame can run.
///
/// At 15 FPS a frame needs 4 ticks, so 8 leaves room for normal slow
/// frames. A frame that needs more than this drops the extra time. See
/// FixedStepClock::Advance().
inline constexpr int kMaxTicksPerFrame = 8;

// Every number that sets how the game feels lives here, in one place.
// Distances are in pixels and times are in seconds, so speeds are pixels per
// second and gravity is pixels per second per second.

/// @brief Width of the player's collision box.
inline constexpr float kPlayerWidth = 22.0f;
/// @brief Height of the player's collision box.
inline constexpr float kPlayerHeight = 28.0f;

/// @brief Horizontal speed while a direction key is held.
inline constexpr float kRunSpeed = 260.0f;
/// @brief Downward acceleration.
inline constexpr float kGravity = 2000.0f;
/// @brief The player never falls faster than this.
inline constexpr float kMaxFallSpeed = 900.0f;

/// @brief Upward speed at the start of a jump.
///
/// With perfect physics the jump would rise v * v / (2 * g) = 108.9 px.
/// The real height is a little lower, and how much lower depends on the
/// size of each step. See Step() in world.cpp.
inline constexpr float kJumpSpeed = 660.0f;
/// @brief "Coyote time": how long after leaving a ledge a jump still works.
///
/// Players press jump a moment late all the time. Without this grace time,
/// a jump pressed just after the feet leave the edge does nothing, and the
/// game feels unfair.
inline constexpr float kCoyoteTime = 0.1f;
/// @brief "Jump buffer": how long before landing a jump press is kept.
///
/// The same idea from the other side: a jump pressed just before the feet
/// touch the ground happens at the moment they do.
inline constexpr float kJumpBufferTime = 0.1f;

/// @brief Width and height of a coin's collision box.
inline constexpr float kCoinSize = 16.0f;

/// @brief How far outside the level the player can go before a respawn.
inline constexpr float kOutOfLevelMargin = 64.0f;

}  // namespace ticker

#endif  // TICKER_TUNING_H_
