#include "clock.h"

namespace ticker {

FixedStepClock::FixedStepClock(double tick_seconds)
    : tick_seconds_(tick_seconds) {}

int FixedStepClock::Advance(double frame_seconds) {
  accumulator_ += frame_seconds;
  int ticks = 0;
  while (accumulator_ >= tick_seconds_) {
    accumulator_ -= tick_seconds_;
    ++ticks;
  }
  return ticks;
}

}  // namespace ticker
