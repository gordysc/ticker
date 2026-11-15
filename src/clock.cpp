#include "clock.h"

#include <cmath>

namespace ticker {

FixedStepClock::FixedStepClock(double tick_seconds, int max_ticks_per_frame)
    : tick_seconds_(tick_seconds), max_ticks_per_frame_(max_ticks_per_frame) {}

int FixedStepClock::Advance(double frame_seconds) {
  accumulator_ += frame_seconds;
  int ticks = 0;
  while (accumulator_ >= tick_seconds_ && ticks < max_ticks_per_frame_) {
    accumulator_ -= tick_seconds_;
    ++ticks;
  }
  // Still a full tick or more left: this frame hit the limit. Keep only the
  // part of a tick that is left over, so the next frame starts fresh
  // instead of owing the same pile of time again.
  if (accumulator_ >= tick_seconds_) {
    const double kept = std::fmod(accumulator_, tick_seconds_);
    dropped_seconds_ += accumulator_ - kept;
    accumulator_ = kept;
  }
  return ticks;
}

}  // namespace ticker
