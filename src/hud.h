#ifndef TICKER_HUD_H_
#define TICKER_HUD_H_

// Header guard; see screen.h for how it works.

#include <span>

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief Everything the debug overlay shows.
///
/// The App fills one of these each frame and passes it to DrawHud(). The
/// overlay itself holds no state and reads no clock, so it only shows what
/// it is given.
struct HudInfo {
  /// @brief Frames per second, as raylib's average over the last second.
  int fps = 0;
  /// @brief Length of the last frame, in milliseconds.
  float frame_ms = 0;
  /// @brief The frame rate cap. 0 means no cap.
  int frame_rate_cap = 0;
  /// @brief True if random short freezes are on.
  bool stutter = false;
  /// @brief True if every Step() is slowed down on purpose.
  bool slow_ticks = false;
  /// @brief The name of the loop mode. A string literal, never null.
  const char* loop_mode = "";
  /// @brief Game time divided by real time. 1 means real speed.
  float game_speed = 1;
  /// @brief How many times Step() ran in the last frame.
  int steps_this_frame = 0;
  /// @brief Time waiting in the accumulator, in milliseconds.
  float accumulator_ms = 0;
  /// @brief The interpolation fraction. 1 if interpolation is off.
  float alpha = 1;
  /// @brief All the time the clock has thrown away, in milliseconds.
  float dropped_ms = 0;
  /// @brief Height of the last jump, in pixels.
  float jump_height = 0;
  /// @brief How many times the player has fallen out of the level.
  int falls = 0;
  /// @brief Recent frame lengths in milliseconds, oldest first.
  ///
  /// A span does not own the numbers. It points at the App's own array, so
  /// it is valid only while DrawHud() runs. Never store it.
  std::span<const float> frame_history_ms;
};

/// @brief Draws the debug overlay in the sidebar right of the level.
/// @param info What to show.
void DrawHud(const HudInfo& info);

}  // namespace ticker

#endif  // TICKER_HUD_H_
