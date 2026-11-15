#include "random.h"

namespace ticker {

// A seed of 0 would give a generator stuck at 0, so it is swapped for a
// fixed non-zero value. Any non-zero value works.
Random::Random(std::uint32_t seed) : state_(seed != 0 ? seed : 0x9E3779B9u) {}

std::uint32_t Random::Next() {
  // Xorshift32 (George Marsaglia, 2003). Each line mixes the bits of the
  // state with a shifted copy of itself. The shift amounts 13, 17 and 5
  // are one of the sets that visit every non-zero 32-bit value before the
  // sequence repeats.
  std::uint32_t x = state_;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  state_ = x;
  return x;
}

int Random::Below(int limit) {
  return static_cast<int>(Next() % static_cast<std::uint32_t>(limit));
}

}  // namespace ticker
