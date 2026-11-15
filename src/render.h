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

/// @brief Draws everything in the world that moves.
/// @param world The world to draw.
void DrawWorld(const World& world);

}  // namespace ticker

#endif  // TICKER_RENDER_H_
