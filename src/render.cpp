#include "render.h"

#include <raylib.h>

namespace ticker {

namespace {

// An unnamed namespace makes everything inside it private to this .cpp
// file, like `static` does for a free function in C.

/// @brief Fill color of a solid tile.
constexpr Color kTileColor{70, 78, 104, 255};
/// @brief Color of the lighter top edge of a solid tile.
constexpr Color kTileEdgeColor{110, 122, 160, 255};
/// @brief Color of the player.
constexpr Color kPlayerColor{255, 196, 87, 255};

}  // namespace

void DrawLevel(const Level& level) {
  for (int row = 0; row < level.rows(); ++row) {
    for (int column = 0; column < level.columns(); ++column) {
      if (!level.IsSolid(column, row)) {
        continue;
      }
      const int x = static_cast<int>(column * kTileSize);
      const int y = static_cast<int>(row * kTileSize);
      const int size = static_cast<int>(kTileSize);
      DrawRectangle(x, y, size, size, kTileColor);
      // A tile with empty space above it is a surface the player can
      // stand on. A light edge on it makes the floors easy to see.
      if (!level.IsSolid(column, row - 1)) {
        DrawRectangle(x, y, size, 3, kTileEdgeColor);
      }
    }
  }
}

void DrawWorld(const World& world) {
  DrawRectangleRec(PlayerBox(world.player), kPlayerColor);
}

}  // namespace ticker
