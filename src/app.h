#ifndef TICKER_APP_H_
#define TICKER_APP_H_

// Header guard; see screen.h for how it works.

#include "level.h"
#include "world.h"

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief The program around the simulation: the game loop, the clock and
/// the drawing.
///
/// World and Step() know nothing about time or windows. App is the part
/// that does: it measures how long each frame took, decides how to advance
/// the world, and draws the result. Each stage of this project changes App
/// and leaves the simulation mostly alone.
///
/// main() creates one App after InitWindow(), then calls Frame() until the
/// window closes.
class App {
 public:
  /// @brief Builds the level and the starting world.
  ///
  /// Call it after InitWindow(), because it reads raylib's clock.
  App();

  /// @brief Not copyable: there is only ever one app.
  App(const App&) = delete;
  /// @brief Not copy-assignable: there is only ever one app.
  App& operator=(const App&) = delete;

  /// @brief Runs one pass of the game loop: measure time, update, draw.
  void Frame();

 private:
  /// @brief Draws the world.
  void Draw() const;

  /// @brief The level. `const`: it never changes after the constructor.
  ///
  /// Members are built in the order they are declared here, not in the
  /// order of the constructor's initializer list. `world_` uses `level_`,
  /// so `level_` has to come first.
  const Level level_;
  /// @brief The simulation state.
  World world_;
  /// @brief The clock time, in seconds, at the start of the last frame.
  ///
  /// A `double` holds about 16 significant digits, a `float` about 7. After
  /// a few hours, a float clock could no longer tell two frames apart.
  double last_frame_start_ = 0;
};

}  // namespace ticker

#endif  // TICKER_APP_H_
