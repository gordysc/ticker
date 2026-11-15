#ifndef TICKER_WORLD_H_
#define TICKER_WORLD_H_

// Header guard; see screen.h for how it works.

#include <raylib.h>

#include "input.h"
#include "level.h"

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
};

/// @brief Makes the world as it is when the game starts.
/// @param level The level to start in.
/// @return A world with the player at the level's start point.
World MakeWorld(const Level& level);

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

}  // namespace ticker

#endif  // TICKER_WORLD_H_
