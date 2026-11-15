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
  /// @brief Jump. True only on the frame the key goes down, so each press
  /// gives one jump.
  bool jump_pressed = false;
};

/// @brief Holds the input from frames until a tick uses it.
///
/// Frames and ticks no longer match one to one. With the accumulator, a
/// fast frame can run no tick, and a slow frame can run several. A key
/// press is seen on one frame only, so without this class:
/// - a press on a frame with no tick is lost: the jump never happens;
/// - a press on a frame with 3 ticks is seen 3 times.
///
/// The latch keeps each press until one tick takes it. Held keys (left and
/// right) are not events, so they simply follow the latest frame.
class InputLatch {
 public:
  /// @brief Adds the input of one frame.
  /// @param frame The keys as read this frame.
  void AddFrame(const Input& frame);

  /// @brief Gives the input for one tick, and uses up the presses in it.
  /// @return The held keys of the latest frame, plus any press that no tick
  ///         has taken yet.
  Input TakeForTick();

 private:
  /// @brief The input that the next tick gets.
  Input pending_;
};

/// @brief Reads the keyboard through raylib.
/// @return The keys that are down now.
Input ReadInput();

}  // namespace ticker

#endif  // TICKER_INPUT_H_
