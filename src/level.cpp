#include "level.h"

#include <cassert>
#include <cstddef>

namespace ticker {

Level::Level(std::span<const std::string_view> rows)
    : columns_(rows.empty() ? 0 : static_cast<int>(rows[0].size())),
      rows_(static_cast<int>(rows.size())) {
  // Reserve the full grid up front, so the vector allocates once instead of
  // growing (and copying itself) as tiles are added.
  solid_.reserve(static_cast<std::size_t>(columns_ * rows_));

  for (int row = 0; row < rows_; ++row) {
    const std::string_view line = rows[static_cast<std::size_t>(row)];
    // assert() stops a debug build with a message if the condition is
    // false. Release builds (with NDEBUG defined) remove the check.
    assert(static_cast<int>(line.size()) == columns_ &&
           "every map row must have the same length");

    for (int column = 0; column < columns_; ++column) {
      const char tile = line[static_cast<std::size_t>(column)];
      solid_.push_back(tile == '#' ? 1 : 0);

      const float left = column * kTileSize;
      const float top = row * kTileSize;
      if (tile == 'P') {
        player_start_ = {left + kTileSize / 2, top + kTileSize};
      } else if (tile == 'c') {
        coin_spawns_.push_back({left + kTileSize / 2, top + kTileSize / 2});
      }
    }
  }
}

bool Level::IsSolid(int column, int row) const {
  if (column < 0 || column >= columns_ || row < 0 || row >= rows_) {
    return false;
  }
  return solid_[static_cast<std::size_t>(row * columns_ + column)] != 0;
}

}  // namespace ticker
