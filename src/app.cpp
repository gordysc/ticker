#include "app.h"

#include <raylib.h>

#include <algorithm>
#include <cstddef>
#include <random>

#include "hud.h"
#include "input.h"
#include "render.h"

namespace ticker {

namespace {

/// @brief A short name for a loop mode, for the sidebar.
/// @param mode The mode.
/// @return A string literal. String literals live for the whole program,
///         so the pointer never dangles.
const char* LoopModeName(LoopMode mode) {
  // There is a `case` for every LoopMode and no `default`. If a new mode
  // is added later, the compiler warns (-Wswitch) that it is missing.
  switch (mode) {
    case LoopMode::Variable:
      return "1. variable timestep";
    case LoopMode::FixedPerFrame:
      return "2. fixed step per frame";
    case LoopMode::Accumulator:
      return "3. accumulator";
    case LoopMode::Interpolated:
      return "4. accumulator + interpolation";
  }
  return "?";
}

/// @brief The mode after this one, back to the first after the last.
/// @param mode The current mode.
/// @return The next mode.
LoopMode NextLoopMode(LoopMode mode) {
  switch (mode) {
    case LoopMode::Variable:
      return LoopMode::FixedPerFrame;
    case LoopMode::FixedPerFrame:
      return LoopMode::Accumulator;
    case LoopMode::Accumulator:
      return LoopMode::Interpolated;
    case LoopMode::Interpolated:
      return LoopMode::Variable;
  }
  return LoopMode::Variable;
}

}  // namespace

// std::random_device asks the operating system for an unpredictable
// number. It only picks the seed. After that, the world's own generator
// makes every random choice, so a known seed gives a known game.
App::App()
    : level_(kLevelMap),
      world_(MakeWorld(level_, std::random_device{}())),
      previous_world_(world_),
      last_frame_start_(GetTime()) {}

void App::Frame() {
  HandleFrameKeys();

  // Measure how long the last frame took: the time from the start of the
  // last frame to the start of this one. That includes the update, the
  // drawing and any time raylib spent waiting to hold the frame rate.
  const double now = GetTime();
  last_frame_seconds_ = now - last_frame_start_;
  last_frame_start_ = now;

  // Drop the oldest entry and add the newest at the end. std::shift_left
  // (C++20) moves every element one place towards the front.
  std::shift_left(frame_history_ms_.begin(), frame_history_ms_.end(), 1);
  frame_history_ms_.back() = static_cast<float>(last_frame_seconds_ * 1000);

  input_latch_.AddFrame(ReadInput());
  const double game_seconds = Update(last_frame_seconds_);
  MeasureGameSpeed(last_frame_seconds_, game_seconds);

  BeginDrawing();
  Draw();
  Stall();
  EndDrawing();
}

double App::Update(double frame_seconds) {
  switch (mode_) {
    case LoopMode::Variable: {
      // The variable timestep: advance the world by exactly as much time
      // as the last frame took. A short frame means a small step, a long
      // frame means a big one.
      const float dt = static_cast<float>(frame_seconds);
      RunStep(dt);
      steps_this_frame_ = 1;
      return dt;
    }
    case LoopMode::FixedPerFrame:
      // One tick per frame, whatever the frame time was. Every step has
      // the same size, so the physics is the same at every frame rate.
      // But the game now runs at the speed of the frames: at 15 FPS it
      // gets 15 ticks (a quarter of a second of game time) every second.
      RunStep(kTickSeconds);
      steps_this_frame_ = 1;
      return kTickSeconds;
    // Two `case` labels in a row share one body. Interpolation changes only
    // the drawing, so the update is the same as in the accumulator mode.
    case LoopMode::Accumulator:
    case LoopMode::Interpolated: {
      // The fixed timestep with an accumulator. The clock says how many
      // whole ticks the real time has paid for: often 1, sometimes 0 on a
      // fast frame, several on a slow one. Every tick has the same dt, and
      // over a second there are always 60 of them.
      const int ticks = clock_.Advance(frame_seconds);
      for (int i = 0; i < ticks; ++i) {
        RunStep(kTickSeconds);
      }
      steps_this_frame_ = ticks;
      return ticks * static_cast<double>(kTickSeconds);
    }
  }
  return 0;
}

void App::RunStep(float dt) {
  // Keep the world as it was before this step. After the last step of a
  // frame, previous_world_ and world_ are the two ticks to draw between.
  previous_world_ = world_;
  Step(world_, level_, input_latch_.TakeForTick(), dt);
  if (slow_ticks_) {
    // Copies a heavy simulation: many enemies, complex physics or a slow
    // computer. One tick now takes longer in real time (25 ms) than the
    // game time it simulates (16.7 ms). The accumulator loop cannot keep
    // up. Each frame takes longer, so the next frame owes more ticks,
    // which makes it take longer still. This is the "spiral of death".
    WaitTime(kSlowTickSeconds);
  }
}

void App::MeasureGameSpeed(double real_seconds, double game_seconds) {
  speed_sample_real_ += real_seconds;
  speed_sample_game_ += game_seconds;
  // One frame is too short to measure: a single long frame would make the
  // number jump about. Half a second of frames gives a steady value.
  if (speed_sample_real_ >= 0.5) {
    game_speed_ = static_cast<float>(speed_sample_game_ / speed_sample_real_);
    speed_sample_real_ = 0;
    speed_sample_game_ = 0;
  }
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
  if (IsKeyPressed(KEY_K)) {
    slow_ticks_ = !slow_ticks_;
  }
  if (IsKeyPressed(KEY_M)) {
    mode_ = NextLoopMode(mode_);
    // Time saved up in another mode does not belong to this one.
    clock_.Reset();
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
  DrawWorld(world_, level_, DrawnPlayerPosition());

  HudInfo hud;
  hud.fps = GetFPS();
  hud.frame_ms = static_cast<float>(last_frame_seconds_ * 1000);
  hud.frame_rate_cap = frame_rate_cap_;
  hud.stutter = stutter_;
  hud.slow_ticks = slow_ticks_;
  hud.loop_mode = LoopModeName(mode_);
  hud.game_speed = game_speed_;
  hud.steps_this_frame = steps_this_frame_;
  hud.accumulator_ms = static_cast<float>(clock_.accumulator() * 1000);
  hud.alpha = mode_ == LoopMode::Interpolated
                  ? static_cast<float>(clock_.Alpha())
                  : 1.0f;
  hud.dropped_ms = static_cast<float>(clock_.dropped_seconds() * 1000);
  hud.jump_height = world_.jump.last_height;
  hud.falls = world_.falls;
  hud.frame_history_ms = frame_history_ms_;
  DrawHud(hud);
}

Vector2 App::DrawnPlayerPosition() const {
  if (mode_ != LoopMode::Interpolated) {
    // Without interpolation, draw the world as the last tick left it. On a
    // frame with no tick, the player is drawn at the same place as on the
    // frame before, and on a frame with 2 ticks it moves twice as far. At
    // frame rates that are not a whole multiple of 60, the motion looks
    // uneven ("judder").
    return world_.player.position;
  }
  // The last tick is up to one tick behind real time. Draw the player
  // part of the way from the tick before it, by the fraction of a tick
  // that real time is ahead. The drawing is then up to one tick (16.7 ms)
  // late, but it moves smoothly at any frame rate.
  return LerpPlayerPosition(previous_world_, world_,
                            static_cast<float>(clock_.Alpha()));
}

}  // namespace ticker
