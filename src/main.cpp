#include <raylib.h>

#include "screen.h"

/// @brief Opens the game window and runs the game loop until it is closed.
/// @return 0 on a normal exit.
int main() {
  InitWindow(ticker::kScreenWidth, ticker::kScreenHeight, "Ticker");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(Color{24, 26, 34, 255});
    DrawText("Ticker", 40, 40, 40, RAYWHITE);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
