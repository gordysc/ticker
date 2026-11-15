#ifndef TICKER_APP_H_
#define TICKER_APP_H_

// Header guard; see screen.h for how it works.

#include <array>

#include "level.h"
#include "random.h"
#include "world.h"

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief The frame rate caps that keys 1 to 5 pick. 0 means no cap.
inline constexpr std::array<int, 5> kFrameRateCaps = {15, 30, 60, 144, 0};
/// @brief How long the H key freezes the program, in seconds.
inline constexpr double kHitchSeconds = 0.25;
/// @brief How many recent frames the frame time graph shows.
inline constexpr int kFrameHistoryLength = 120;

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
  /// @brief Reads the keys that change the frame rate, not the game.
  void HandleFrameKeys();

  /// @brief Freezes the program for a moment if a hitch or stutter is due.
  ///
  /// This copies what happens on a real computer when the operating system
  /// runs something else, a file loads, or the window is dragged.
  void Stall();

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
  /// @brief How long the last frame took, in seconds.
  double last_frame_seconds_ = 0;
  /// @brief Recent frame lengths in milliseconds, oldest first.
  std::array<float, kFrameHistoryLength> frame_history_ms_{};

  /// @brief The frame rate cap now in use. 0 means no cap.
  int frame_rate_cap_ = 60;
  /// @brief True if the next frame freezes for kHitchSeconds.
  bool hitch_due_ = false;
  /// @brief True if random short freezes are on.
  bool stutter_ = false;
  /// @brief Picks when the stutter freezes happen, and for how long.
  ///
  /// This is a different generator from the one in the World. The stutter
  /// belongs to the program around the game, not to the game. If it used
  /// the world's generator, turning the stutter on would change where the
  /// coins appear.
  Random stutter_random_{12345};
};

}  // namespace ticker

#endif  // TICKER_APP_H_
