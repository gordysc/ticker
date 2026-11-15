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

}  // namespace ticker
