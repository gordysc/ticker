#ifndef TICKER_INPUT_H_
#define TICKER_INPUT_H_

// Header guard; see screen.h for how it works.

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief What the player asked for.
///
/// The game reads the keyboard in one place, ReadInput(), and passes this
/// struct to the simulation. The simulation never calls raylib's
/// IsKeyDown() itself, so a test can drive it with a hand-made Input and no
/// window at all.
struct Input {
  /// @brief Run left. Held.
  bool left = false;
  /// @brief Run right. Held.
  bool right = false;
};

/// @brief Reads the keyboard through raylib.
/// @return The keys that are down now.
Input ReadInput();

}  // namespace ticker

#endif  // TICKER_INPUT_H_
