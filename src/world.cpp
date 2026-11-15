#include "world.h"

#include <algorithm>
#include <cmath>

#include "tuning.h"

namespace ticker {

namespace {

/// @brief A block of tiles: every column and row from the first to the last.
struct TileRange {
  /// @brief Leftmost column.
  int first_column;
  /// @brief Rightmost column.
  int last_column;
  /// @brief Top row.
  int first_row;
  /// @brief Bottom row.
  int last_row;
};

/// @brief Finds the tiles that a box covers.
///
/// A box whose right edge sits exactly on a tile boundary does not cover
/// the next tile. That is why the right and bottom edges use ceil() - 1
/// instead of floor(): a player standing on a floor touches it but does not
/// overlap it.
///
/// @param box The box, in pixels.
/// @return The tiles under the box.
TileRange TilesUnder(Rectangle box) {
  return {
      static_cast<int>(std::floor(box.x / kTileSize)),
      static_cast<int>(std::ceil((box.x + box.width) / kTileSize)) - 1,
      static_cast<int>(std::floor(box.y / kTileSize)),
      static_cast<int>(std::ceil((box.y + box.height) / kTileSize)) - 1,
  };
}

/// @brief Tells if a box overlaps any solid tile.
/// @param level The level.
/// @param box The box, in pixels.
/// @return True if at least one solid tile is under the box.
bool OverlapsSolid(const Level& level, Rectangle box) {
  const TileRange tiles = TilesUnder(box);
  for (int row = tiles.first_row; row <= tiles.last_row; ++row) {
    for (int column = tiles.first_column; column <= tiles.last_column;
         ++column) {
      if (level.IsSolid(column, row)) {
        return true;
      }
    }
  }
  return false;
}

// The two Move functions below use a "move, then push out" collision test.
// They move the box by the full distance, then look for solid tiles at the
// new place only. They never look at the tiles between the old place and the
// new place. If one step moves the box further than a tile is thick, the box
// can jump over the tile completely. That is "tunneling", and stage 1 lets
// you see it happen.

/// @brief Moves the player sideways and stops it at walls.
/// @param player The player to move.
/// @param level The level.
/// @param dx Pixels to move. Negative is left.
void MoveX(Player& player, const Level& level, float dx) {
  if (dx == 0) {
    return;
  }
  player.position.x += dx;
  const TileRange tiles = TilesUnder(PlayerBox(player));
  // Look at the columns nearest the player first. The box did not overlap
  // anything before it moved, so the first solid tile found is the wall
  // it ran into.
  const int step = dx > 0 ? 1 : -1;
  const int start = dx > 0 ? tiles.first_column : tiles.last_column;
  const int end = dx > 0 ? tiles.last_column + 1 : tiles.first_column - 1;
  for (int column = start; column != end; column += step) {
    for (int row = tiles.first_row; row <= tiles.last_row; ++row) {
      if (level.IsSolid(column, row)) {
        // Push the box back out, so its edge touches the wall.
        player.position.x = dx > 0 ? column * kTileSize - kPlayerWidth
                                   : (column + 1) * kTileSize;
        player.velocity.x = 0;
        return;
      }
    }
  }
}

/// @brief Moves the player up or down and stops it at floors and ceilings.
/// @param player The player to move.
/// @param level The level.
/// @param dy Pixels to move. Negative is up.
void MoveY(Player& player, const Level& level, float dy) {
  if (dy == 0) {
    return;
  }
  player.position.y += dy;
  const TileRange tiles = TilesUnder(PlayerBox(player));
  // The same search as MoveX(), with rows instead of columns.
  const int step = dy > 0 ? 1 : -1;
  const int start = dy > 0 ? tiles.first_row : tiles.last_row;
  const int end = dy > 0 ? tiles.last_row + 1 : tiles.first_row - 1;
  for (int row = start; row != end; row += step) {
    for (int column = tiles.first_column; column <= tiles.last_column;
         ++column) {
      if (level.IsSolid(column, row)) {
        player.position.y = dy > 0 ? row * kTileSize - kPlayerHeight
                                   : (row + 1) * kTileSize;
        player.velocity.y = 0;
        return;
      }
    }
  }
}

/// @brief Puts the player back at the start of the level.
/// @param world The world.
/// @param level The level.
void Respawn(World& world, const Level& level) {
  const Vector2 start = level.player_start();
  world.player = Player{};
  world.player.position = {start.x - kPlayerWidth / 2, start.y - kPlayerHeight};
}

}  // namespace

World MakeWorld(const Level& level) {
  World world;
  Respawn(world, level);
  return world;
}

void Step(World& world, const Level& level, const Input& input, float dt) {
  Player& player = world.player;

  // The bool-to-int conversion gives 1 for true and 0 for false, so this
  // is -1, 0 or +1.
  const int direction = int{input.right} - int{input.left};
  player.velocity.x = direction * kRunSpeed;

  // "Semi-implicit Euler" integration: change the velocity first, then
  // move by the new velocity. It is simple and stable, but it is only an
  // estimate of real motion, and the size of the error depends on dt.
  player.velocity.y = std::min(player.velocity.y + kGravity * dt, kMaxFallSpeed);

  MoveX(player, level, player.velocity.x * dt);
  MoveY(player, level, player.velocity.y * dt);

  // Look 0.5 px below the feet. A plain "did MoveY() hit a floor?" test
  // fails at very high frame rates: dy gets so small that floating-point
  // rounding leaves the box exactly on the floor, but not in it.
  Rectangle below = PlayerBox(player);
  below.y += 0.5f;
  player.on_ground = player.velocity.y >= 0 && OverlapsSolid(level, below);

  // A player who tunnels through the outer wall falls out of the level.
  const Vector2 size = level.size();
  const Vector2 p = player.position;
  if (p.x < -kOutOfLevelMargin || p.x > size.x + kOutOfLevelMargin ||
      p.y < -kOutOfLevelMargin || p.y > size.y + kOutOfLevelMargin) {
    Respawn(world, level);
    ++world.falls;
  }
}

Rectangle PlayerBox(const Player& player) {
  return {player.position.x, player.position.y, kPlayerWidth, kPlayerHeight};
}

}  // namespace ticker
