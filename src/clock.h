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
  /// @param max_ticks_per_frame The most ticks that Advance() gives for one
  ///        frame. See Advance().
  FixedStepClock(double tick_seconds, int max_ticks_per_frame);

  /// @brief Adds one frame of real time and takes out the ticks it pays for.
  ///
  /// If the accumulator holds more than `max_ticks_per_frame` ticks, the
  /// clock gives only that many and throws away the rest of the time. The
  /// game then runs slower than real time for a moment, but each frame
  /// does a known, limited amount of work. Without the limit, a computer
  /// too slow to run the ticks gets further behind each frame, until the
  /// game freezes (the "spiral of death").
  ///
  /// @param frame_seconds How long the frame took.
  /// @return How many ticks to run this frame, from 0 to
  ///         `max_ticks_per_frame`.
  int Advance(double frame_seconds);

  /// @brief Throws away the time in the accumulator.
  void Reset() { accumulator_ = 0; }

  /// @brief Time that is waiting for the next tick.
  /// @return Seconds, from 0 up to (but not including) one tick.
  double accumulator() const { return accumulator_; }

  /// @brief Length of one tick.
  /// @return Seconds.
  double tick_seconds() const { return tick_seconds_; }

  /// @brief All the time that Advance() has thrown away so far.
  /// @return Seconds.
  double dropped_seconds() const { return dropped_seconds_; }

 private:
  /// @brief Length of one tick, in seconds.
  double tick_seconds_;
  /// @brief See the constructor.
  int max_ticks_per_frame_;
  /// @brief See dropped_seconds().
  double dropped_seconds_ = 0;
  /// @brief Real time not yet turned into ticks, in seconds.
  ///
  /// A `double`, like the frame times from raylib's GetTime(), so no
  /// precision is lost when a frame time is added.
  double accumulator_ = 0;
};

}  // namespace ticker

#endif  // TICKER_CLOCK_H_
