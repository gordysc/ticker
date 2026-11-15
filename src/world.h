#ifndef TICKER_WORLD_H_
#define TICKER_WORLD_H_

// Header guard; see screen.h for how it works.

#include <raylib.h>

#include <cstdint>

#include "input.h"
#include "level.h"
#include "random.h"

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief The one character the player controls.
struct Player {
  /// @brief Top-left corner of the collision box, in pixels.
  Vector2 position{};
  /// @brief Pixels per second. Positive y is down, as on the screen.
  Vector2 velocity{};
  /// @brief True if solid ground is directly below the player.
  bool on_ground = false;
  /// @brief Seconds of coyote time left. See kCoyoteTime.
  float coyote_time_left = 0;
  /// @brief Seconds left to use the last jump press. See kJumpBufferTime.
  float jump_buffer_left = 0;
};

/// @brief Measures each jump, so the debug overlay can show how high it went.
struct JumpStats {
  /// @brief True from the start of a jump until the player stops rising.
  bool rising = false;
  /// @brief The y of the feet when the jump started.
  float start_feet = 0;
  /// @brief The smallest y of the feet so far: the highest point.
  float highest_feet = 0;
  /// @brief Height of the last finished jump, in pixels.
  float last_height = 0;
};

/// @brief The simulation state: everything that Step() can change.
///
/// World is a plain struct of values with no pointers, so a copy of it is
/// a full, separate snapshot of the game. Later stages depend on that.
struct World {
  /// @brief The player.
  Player player;
  /// @brief How many times the player has left the level and respawned.
  int falls = 0;
  /// @brief Which coin spawn point holds the coin: an index into
  /// Level::coin_spawns(). There is always exactly one coin.
  int coin = 0;
  /// @brief Number of coins collected.
  int score = 0;
  /// @brief Picks where the next coin appears.
  ///
  /// The generator is part of the world, not a global. Its state changes
  /// only inside Step(), so the same inputs always give the same coins.
  Random random{1};
  /// @brief Measurements of the jumps.
  JumpStats jump;
};

/// @brief Makes the world as it is when the game starts.
/// @param level The level to start in.
/// @param seed Seed for the world's random number generator.
/// @return A world with the player at the level's start point.
World MakeWorld(const Level& level, std::uint32_t seed);

/// @brief Advances the simulation by `dt` seconds.
///
/// This is the whole game logic. It reads no clock and no keyboard: the
/// caller decides how much time passes and what the player pressed. That
/// split is the base of every later stage.
///
/// @param world The state to change. Taken by non-const reference, because
///        the function changes it in place.
/// @param level The level the player moves in.
/// @param input What the player asked for.
/// @param dt How many seconds to advance.
void Step(World& world, const Level& level, const Input& input, float dt);

/// @brief The player's collision box.
/// @param player The player.
/// @return The box in pixels.
Rectangle PlayerBox(const Player& player);

/// @brief Where to draw the player between two ticks.
///
/// Mixes the positions of two snapshots of the world in a straight line.
/// "Lerp" is short for "linear interpolation".
///
/// @param previous The world before the last tick.
/// @param current The world after the last tick.
/// @param alpha How far to go from previous to current: 0 gives the
///        previous position, 1 gives the current one.
/// @return The top-left corner of the player box, in pixels.
Vector2 LerpPlayerPosition(const World& previous, const World& current,
                           float alpha);

/// @brief The collision box of the coin.
/// @param world The world.
/// @param level The level, which holds the coin spawn points.
/// @return The box in pixels.
Rectangle CoinBox(const World& world, const Level& level);

}  // namespace ticker

#endif  // TICKER_WORLD_H_
