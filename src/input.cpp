#include "input.h"

#include <raylib.h>

namespace ticker {

Input ReadInput() {
  Input input;
  input.left = IsKeyDown(KEY_LEFT);
  input.right = IsKeyDown(KEY_RIGHT);
  // IsKeyPressed() is true only on the first frame a key is down.
  // IsKeyDown() is true on every frame while it is held.
  input.jump_pressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP);
  return input;
}

void InputLatch::AddFrame(const Input& frame) {
  pending_.left = frame.left;
  pending_.right = frame.right;
  // `||=` does not exist in C++, but `|=` on bools works: true stays true.
  // A press waits here until a tick takes it, across any number of frames.
  pending_.jump_pressed |= frame.jump_pressed;
}

Input InputLatch::TakeForTick() {
  // Return a copy, then clear the press in the stored one, so the next
  // tick in the same frame does not see it again.
  const Input tick = pending_;
  pending_.jump_pressed = false;
  return tick;
}

}  // namespace ticker
