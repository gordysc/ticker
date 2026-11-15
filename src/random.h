#ifndef TICKER_RANDOM_H_
#define TICKER_RANDOM_H_

// Header guard; see screen.h for how it works.

#include <cstdint>

/// @brief Everything that belongs to the game.
namespace ticker {

/// @brief A small random number generator with a fully known output.
///
/// The C++ standard library has good generators, but its distributions
/// (such as std::uniform_int_distribution) are not fully specified. The
/// same seed can give different numbers with GCC's library and with
/// Clang's library. A replay recorded on one computer then plays back
/// differently on another one. This class uses the "xorshift32" method,
/// which is a few lines of plain integer maths, so its output is the same
/// everywhere.
///
/// Random holds a single integer, so copying it copies the exact place in
/// the sequence. A copy of the World therefore also copies its future
/// random numbers.
class Random {
 public:
  /// @brief Creates a generator.
  ///
  /// `explicit` stops the compiler from silently turning a plain number into
  /// a Random, for example when a function that takes a `Random` is called
  /// with an int by mistake.
  ///
  /// @param seed Starting value. The same seed always gives the same
  ///        sequence of numbers.
  explicit Random(std::uint32_t seed);

  /// @brief Returns the next number in the sequence.
  /// @return Any 32-bit value except 0.
  std::uint32_t Next();

  /// @brief Returns a random int below a limit.
  ///
  /// The `%` makes small numbers a tiny bit more likely than large ones,
  /// because 2^32 is not an exact multiple of most limits. For a limit of a
  /// few dozen, the difference is far too small to matter in a game.
  ///
  /// @param limit Number of possible results. Must be 1 or more.
  /// @return A value from 0 up to (but not including) `limit`.
  int Below(int limit);

  /// @brief The generator's internal state.
  /// @return The value the next call to Next() starts from.
  std::uint32_t state() const { return state_; }

 private:
  /// @brief The last number made. Xorshift never makes 0 from a non-zero
  /// state, and never leaves 0 once there, so 0 is not allowed.
  std::uint32_t state_;
};

}  // namespace ticker

#endif  // TICKER_RANDOM_H_
