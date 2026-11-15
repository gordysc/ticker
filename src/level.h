#ifndef TICKER_LEVEL_H_
#define TICKER_LEVEL_H_

// Header guard; see screen.h for how it works.

#include <raylib.h>

#include <array>
#include <span>
#include <string_view>
#include <vector>

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief Width and height of one tile, in pixels.
inline constexpr float kTileSize = 32.0f;

/// @brief The level as text. One character is one tile.
///
/// - `#` is a solid tile.
/// - `.` is empty space.
/// - `P` is where the player starts (empty space).
/// - `c` is a place where a coin can appear (empty space).
///
/// The block of `#` in the bottom-left corner is exactly 3 tiles (96 px)
/// tall. It is a ruler: at 60 FPS the player can jump onto it, but at
/// 15 FPS the same jump falls short. Every other step is 2 tiles tall, so
/// the rest of the level works at any frame rate.
///
/// `std::string_view` is a pointer and a length that look at characters
/// someone else owns. It never copies or frees them. String literals live
/// for the whole program, so views of them never dangle, which makes this
/// table safe to keep as a `constexpr` global.
inline constexpr std::array<std::string_view, 22> kLevelMap = {
    "##############################",  //
    "#............................#",  //
    "#............................#",  //
    "#............................#",  //
    "#..c.........................#",  //
    "#.####.......................#",  //
    "#.......c....................#",  //
    "#......####..................#",  //
    "#.............c..............#",  //
    "#............####............#",  //
    "#...................c........#",  //
    "#..................####......#",  //
    "#..........................c.#",  //
    "#........................#####",  //
    "#............................#",  //
    "#....................####....#",  //
    "#................c...........#",  //
    "#.c............####..........#",  //
    "####......c..................#",  //
    "####.....####................#",  //
    "####..P..................c...#",  //
    "##############################",  //
};

/// @brief A grid of solid and empty tiles, plus the marked places in it.
///
/// The level never changes while the game runs. The simulation reads it,
/// but it is not part of the simulation state. This matters later: a
/// replay or a saved copy of the world only has to hold what can change.
class Level {
 public:
  /// @brief Builds a level from a text map such as kLevelMap.
  ///
  /// `std::span` is a view of a run of elements (here, of the rows), like
  /// `std::string_view` is a view of characters. It lets this constructor
  /// take a `std::array`, a `std::vector` or a C array without a copy.
  ///
  /// @param rows The map, one string per row, top row first. Every row
  ///        must have the same length.
  explicit Level(std::span<const std::string_view> rows);

  /// @brief Tells if a tile is solid.
  /// @param column Tile column, 0 at the left.
  /// @param row Tile row, 0 at the top.
  /// @return True for a solid tile. A tile outside the map is empty, so a
  ///         player who gets through the outer wall falls out of the level.
  bool IsSolid(int column, int row) const;

  /// @brief Number of tile columns.
  /// @return The map width in tiles.
  int columns() const { return columns_; }

  /// @brief Number of tile rows.
  /// @return The map height in tiles.
  int rows() const { return rows_; }

  /// @brief The level size in pixels.
  /// @return Width and height.
  Vector2 size() const {
    return {columns_ * kTileSize, rows_ * kTileSize};
  }

  /// @brief Where the player starts.
  /// @return The middle of the bottom edge of the `P` tile, in pixels. The
  ///         player stands with its feet on this point.
  Vector2 player_start() const { return player_start_; }

  /// @brief The places where coins can appear.
  ///
  /// Returns a const reference, so the caller reads the level's own vector
  /// without a copy. The reference is valid while the Level exists.
  ///
  /// @return The middle of every `c` tile, in pixels, in map order.
  const std::vector<Vector2>& coin_spawns() const { return coin_spawns_; }

 private:
  /// @brief Number of tile columns.
  int columns_ = 0;
  /// @brief Number of tile rows.
  int rows_ = 0;
  /// @brief One entry per tile, row by row: true if the tile is solid.
  ///
  /// `std::vector<bool>` is a special case in the standard library that
  /// packs 8 values into a byte. A `std::vector<char>` avoids its odd
  /// behavior (it gives no real `bool&` references) for no real cost here.
  std::vector<char> solid_;
  /// @brief See player_start().
  Vector2 player_start_{};
  /// @brief See coin_spawns().
  std::vector<Vector2> coin_spawns_;
};

}  // namespace ticker

#endif  // TICKER_LEVEL_H_
