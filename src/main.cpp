#include <raylib.h>

#include "app.h"
#include "screen.h"

/// @brief Opens the game window and runs the game loop until it is closed.
/// @return 0 on a normal exit.
int main() {
  InitWindow(ticker::kScreenWidth, ticker::kScreenHeight, "Ticker");
  SetTargetFPS(60);

  // The extra `{ }` block makes `app` go out of scope, and its destructor
  // run, before CloseWindow(). Anything the App owns that needs the window
  // (textures, fonts) is then freed while the window still exists.
  {
    ticker::App app;
    while (!WindowShouldClose()) {
      app.Frame();
    }
  }

  CloseWindow();
  return 0;
}
