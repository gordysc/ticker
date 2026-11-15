#include "hud.h"

#include <raylib.h>

#include <algorithm>
#include <array>
#include <cstddef>

#include "screen.h"

namespace ticker {

namespace {

/// @brief Left edge of the overlay panel, in pixels.
constexpr int kPanelX = kLevelAreaWidth;
/// @brief Top edge of the overlay panel, in pixels.
constexpr int kPanelY = 0;
/// @brief Width of the overlay panel, in pixels.
constexpr int kPanelWidth = kSidebarWidth;
/// @brief Space between the panel edge and its contents, in pixels.
constexpr int kPadding = 16;
/// @brief Font size of the overlay text, in pixels.
constexpr int kFontSize = 16;
/// @brief Distance from one text line to the next, in pixels.
constexpr int kLineHeight = 20;
/// @brief Height of the frame time graph, in pixels.
constexpr int kGraphHeight = 60;
/// @brief Milliseconds that fill the full height of the graph.
constexpr float kGraphMaxMs = 100.0f;

/// @brief Writes lines of text one under the other.
///
/// A small helper class keeps the "where does the next line go" sum in one
/// place, so the code that draws the overlay is just a list of lines.
class TextLines {
 public:
  /// @brief Starts at a place on the screen.
  /// @param x Left edge of the text.
  /// @param y Top of the first line.
  TextLines(int x, int y) : x_(x), y_(y) {}

  /// @brief Writes one line and moves down.
  /// @param text The line. Taken as `const char*` because raylib's
  ///        DrawText() and TextFormat() use C strings.
  /// @param color The text color.
  void Add(const char* text, Color color = RAYWHITE) {
    DrawText(text, x_, y_, kFontSize, color);
    y_ += kLineHeight;
  }

  /// @brief The top of the next line.
  /// @return The y in pixels.
  int y() const { return y_; }

 private:
  /// @brief Left edge of the text.
  int x_;
  /// @brief Top of the next line.
  int y_;
};

/// @brief Draws recent frame times as a bar graph.
/// @param history Frame lengths in milliseconds, oldest first.
/// @param x Left edge of the graph.
/// @param y Top of the graph.
/// @param width Width of the graph.
void DrawFrameGraph(std::span<const float> history, int x, int y, int width) {
  DrawRectangle(x, y, width, kGraphHeight, Color{0, 0, 0, 120});
  if (history.empty()) {
    return;
  }
  const float bar_width = static_cast<float>(width) / history.size();
  // A range-based for loop does not give the index, so count it by hand.
  std::size_t i = 0;
  for (const float ms : history) {
    const float height = std::min(ms, kGraphMaxMs) / kGraphMaxMs * kGraphHeight;
    // Frames longer than a 30 FPS frame show in red.
    const Color color =
        ms > 34.0f ? Color{240, 90, 90, 255} : Color{120, 200, 240, 255};
    DrawRectangleRec({x + i * bar_width, y + kGraphHeight - height,
                      std::max(bar_width - 1, 1.0f), height},
                     color);
    ++i;
  }
  // A guide line at 16.7 ms, the length of one 60 FPS frame.
  const int line_y =
      y + kGraphHeight - static_cast<int>(16.7f / kGraphMaxMs * kGraphHeight);
  DrawLine(x, line_y, x + width, line_y, Color{255, 255, 255, 90});
}

}  // namespace

void DrawHud(const HudInfo& info) {
  DrawRectangle(kPanelX, kPanelY, kPanelWidth, kScreenHeight,
                Color{14, 16, 22, 255});

  TextLines lines(kPanelX + kPadding, kPanelY + kPadding);
  // TextFormat() returns a pointer into a buffer inside raylib, not a
  // string you own. raylib has only 4 such buffers and reuses them in
  // turn, so `cap` stays valid for the next few calls but not longer.
  const char* cap =
      info.frame_rate_cap > 0 ? TextFormat("%d", info.frame_rate_cap) : "none";
  lines.Add(TextFormat("FPS %d   frame %.1f ms   cap %s", info.fps,
                       info.frame_ms, cap));
  lines.Add(TextFormat("stutter %s", info.stutter ? "ON" : "off"),
            info.stutter ? Color{240, 90, 90, 255} : RAYWHITE);
  lines.Add(TextFormat("slow ticks %s", info.slow_ticks ? "ON (25 ms)" : "off"),
            info.slow_ticks ? Color{240, 90, 90, 255} : RAYWHITE);
  lines.Add(TextFormat("loop %s", info.loop_mode), Color{255, 196, 87, 255});
  // A game speed more than 2% away from 1 shows in red.
  const bool speed_off = info.game_speed < 0.98f || info.game_speed > 1.02f;
  lines.Add(TextFormat("game speed x%.2f", info.game_speed),
            speed_off ? Color{240, 90, 90, 255} : RAYWHITE);
  lines.Add(TextFormat("steps this frame %d   accumulator %.1f ms",
                       info.steps_this_frame, info.accumulator_ms));
  // The 3-tile block in the corner is 96 px tall.
  lines.Add(TextFormat("last jump %.1f px  (block: 96)", info.jump_height),
            info.jump_height >= 96.0f ? Color{120, 220, 160, 255}
                                      : Color{240, 170, 90, 255});
  lines.Add(TextFormat("falls out of the level: %d", info.falls));

  DrawFrameGraph(info.frame_history_ms, kPanelX + kPadding,
                 lines.y() + kPadding / 2, kPanelWidth - kPadding * 2);

  // The key list sits at the bottom of the sidebar. A `constexpr` array of
  // `const char*` is built at compile time, and .size() is known there too.
  constexpr Color kDim{150, 156, 175, 255};
  constexpr std::array<const char*, 6> kKeys = {
      "Left / Right: run      Space / Up: jump",
      "M: next loop mode",
      "1-5: frame rate cap",
      "H: one 250 ms hitch",
      "J: random stutter on / off",
      "K: slow ticks on / off",
  };
  TextLines keys(
      kPanelX + kPadding,
      kScreenHeight - kPadding - static_cast<int>(kKeys.size()) * kLineHeight);
  for (const char* key : kKeys) {
    keys.Add(key, kDim);
  }
}

}  // namespace ticker
