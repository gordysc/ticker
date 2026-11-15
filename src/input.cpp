#include "input.h"

#include <raylib.h>

namespace ticker {

Input ReadInput() {
  Input input;
  input.left = IsKeyDown(KEY_LEFT);
  input.right = IsKeyDown(KEY_RIGHT);
  return input;
}

}  // namespace ticker
