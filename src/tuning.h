#ifndef TICKER_TUNING_H_
#define TICKER_TUNING_H_

// Header guard; see screen.h for how it works.

/// @brief Everything that belongs to the game.
namespace ticker {

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

/// @brief How far outside the level the player can go before a respawn.
inline constexpr float kOutOfLevelMargin = 64.0f;

}  // namespace ticker

#endif  // TICKER_TUNING_H_
