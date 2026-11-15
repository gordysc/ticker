#include "app.h"

#include <raylib.h>

#include <algorithm>
#include <cstddef>
#include <random>

#include "hud.h"
#include "input.h"
#include "render.h"

namespace ticker {

// std::random_device asks the operating system for an unpredictable
// number. It only picks the seed. After that, the world's own generator
// makes every random choice, so a known seed gives a known game.
App::App()
    : level_(kLevelMap),
      world_(MakeWorld(level_, std::random_device{}())),
      last_frame_start_(GetTime()) {}

void App::Frame() {
  HandleFrameKeys();

  // Measure how long the last frame took: the time from the start of the
  // last frame to the start of this one. That includes the update, the
  // drawing and any time raylib spent waiting to hold the frame rate.
  const double now = GetTime();
  last_frame_seconds_ = now - last_frame_start_;
  last_frame_start_ = now;
  const float dt = static_cast<float>(last_frame_seconds_);

  // Drop the oldest entry and add the newest at the end. std::shift_left
  // (C++20) moves every element one place towards the front.
  std::shift_left(frame_history_ms_.begin(), frame_history_ms_.end(), 1);
  frame_history_ms_.back() = static_cast<float>(last_frame_seconds_ * 1000);

  // The variable timestep: advance the world by exactly as much time as
  // the last frame took. A short frame means a small step, a long frame
  // means a big one.
  Step(world_, level_, ReadInput(), dt);

  BeginDrawing();
  Draw();
  Stall();
  EndDrawing();
}

void App::HandleFrameKeys() {
  // KEY_ONE to KEY_FIVE are next to each other in raylib's KeyboardKey
  // enum, so KEY_ONE + i is the key for cap number i.
  for (int i = 0; i < static_cast<int>(kFrameRateCaps.size()); ++i) {
    if (IsKeyPressed(KEY_ONE + i)) {
      frame_rate_cap_ = kFrameRateCaps[static_cast<std::size_t>(i)];
      // raylib waits at the end of each frame (inside EndDrawing()) until
      // 1 / cap seconds have passed. A cap of 0 turns the wait off, so the
      // game runs as fast as the computer can go.
      SetTargetFPS(frame_rate_cap_);
    }
  }
  if (IsKeyPressed(KEY_H)) {
    hitch_due_ = true;
  }
  if (IsKeyPressed(KEY_J)) {
    stutter_ = !stutter_;
  }
}

void App::Stall() {
  double seconds = 0;
  if (hitch_due_) {
    seconds = kHitchSeconds;
    hitch_due_ = false;
  } else if (stutter_ && stutter_random_.Below(8) == 0) {
    // About one frame in 8 takes an extra 50 to 150 ms.
    seconds = 0.05 + stutter_random_.Below(101) / 1000.0;
  }
  if (seconds > 0) {
    // WaitTime() sleeps. The next frame measures the long gap and passes
    // a big dt to Step().
    WaitTime(seconds);
  }
}

void App::Draw() const {
  ClearBackground(Color{24, 26, 34, 255});
  DrawLevel(level_);
  DrawWorld(world_, level_);

  HudInfo hud;
  hud.fps = GetFPS();
  hud.frame_ms = static_cast<float>(last_frame_seconds_ * 1000);
  hud.frame_rate_cap = frame_rate_cap_;
  hud.stutter = stutter_;
  hud.jump_height = world_.jump.last_height;
  hud.falls = world_.falls;
  hud.frame_history_ms = frame_history_ms_;
  DrawHud(hud);
}

}  // namespace ticker
