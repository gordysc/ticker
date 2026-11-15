#include "app.h"

#include <raylib.h>

#include <random>

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
  // Measure how long the last frame took: the time from the start of the
  // last frame to the start of this one. That includes the update, the
  // drawing and any time raylib spent waiting to hold the frame rate.
  const double now = GetTime();
  const float dt = static_cast<float>(now - last_frame_start_);
  last_frame_start_ = now;

  // The variable timestep: advance the world by exactly as much time as
  // the last frame took. A short frame means a small step, a long frame
  // means a big one.
  Step(world_, level_, ReadInput(), dt);

  BeginDrawing();
  Draw();
  EndDrawing();
}

void App::Draw() const {
  ClearBackground(Color{24, 26, 34, 255});
  DrawLevel(level_);
  DrawWorld(world_, level_);
}

}  // namespace ticker
