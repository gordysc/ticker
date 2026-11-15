#ifndef TICKER_RENDER_H_
#define TICKER_RENDER_H_

// Header guard; see screen.h for how it works.

#include "level.h"
#include "world.h"

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief Draws every solid tile of the level.
///
/// Drawing only reads the level, so it takes a const reference: no copy,
/// and the compiler stops any accidental change.
///
/// @param level The level to draw.
void DrawLevel(const Level& level);

/// @brief Draws the player, the coin and the score.
/// @param world The world to draw.
/// @param level The level, which holds the coin spawn points.
void DrawWorld(const World& world, const Level& level);

}  // namespace ticker

#endif  // TICKER_RENDER_H_
