#ifndef TICKER_CLOCK_H_
#define TICKER_CLOCK_H_

// Header guard; see screen.h for how it works.

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief Turns real frame times into a whole number of fixed ticks.
///
/// This is the "accumulator" from Glenn Fiedler's article "Fix Your
/// Timestep!". Each frame adds its real length to the accumulator. Each
/// tick takes one tick length out of it. Time that is too short for a full
/// tick stays in the accumulator for the next frame, so no time is lost.
///
/// A fast frame (say 7 ms at 144 FPS) often gives 0 ticks. A slow frame
/// (say 67 ms at 15 FPS) gives 4. Over a second, the total is always 60
/// ticks: the game runs at real speed at any frame rate.
///
/// The clock only counts. It does not call Step(), and it reads no real
/// clock, so a test can feed it any frame times it wants.
class FixedStepClock {
 public:
  /// @brief Creates a clock with an empty accumulator.
  /// @param tick_seconds Length of one tick, in seconds.
  explicit FixedStepClock(double tick_seconds);

  /// @brief Adds one frame of real time and takes out the ticks it pays for.
  /// @param frame_seconds How long the frame took.
  /// @return How many ticks to run this frame. Can be 0.
  int Advance(double frame_seconds);

  /// @brief Throws away the time in the accumulator.
  void Reset() { accumulator_ = 0; }

  /// @brief Time that is waiting for the next tick.
  /// @return Seconds, from 0 up to (but not including) one tick.
  double accumulator() const { return accumulator_; }

  /// @brief Length of one tick.
  /// @return Seconds.
  double tick_seconds() const { return tick_seconds_; }

 private:
  /// @brief Length of one tick, in seconds.
  double tick_seconds_;
  /// @brief Real time not yet turned into ticks, in seconds.
  ///
  /// A `double`, not a `float`. Small errors in this sum add up over a long
  /// game, and a float would lose a tick every few minutes.
  double accumulator_ = 0;
};

}  // namespace ticker

#endif  // TICKER_CLOCK_H_
