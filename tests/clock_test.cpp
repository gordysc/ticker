// Tests for FixedStepClock. The clock reads no real time, so the tests feed
// it made-up frame times and count the ticks it gives back.

#include "clock.h"

#include <cmath>
#include <vector>

#include "check.h"

namespace ticker {
namespace {

/// @brief One tick at 60 ticks per second.
constexpr double kTick = 1.0 / 60.0;
/// @brief The most ticks per frame, for every clock in these tests.
constexpr int kMaxTicks = 8;

/// @brief Checks that two doubles are equal to within a small tolerance.
/// @param a First value.
/// @param b Second value.
/// @return True if they differ by less than a microsecond.
bool Near(double a, double b) { return std::fabs(a - b) < 1e-6; }

/// @brief Counts the ticks for a number of equal frames.
/// @param clock The clock to advance.
/// @param frame_seconds Length of each frame.
/// @param frames Number of frames.
/// @return Total ticks over all the frames.
int TotalTicks(FixedStepClock& clock, double frame_seconds, int frames) {
  int total = 0;
  for (int i = 0; i < frames; ++i) {
    total += clock.Advance(frame_seconds);
  }
  return total;
}

/// @brief One second of frames gives 60 ticks at any frame rate.
void TestSixtyTicksPerSecondAtAnyFrameRate() {
  for (const int fps : {15, 30, 60, 144, 1000}) {
    FixedStepClock clock(kTick, kMaxTicks);
    // A little extra time absorbs the rounding in 1.0 / fps.
    const int ticks = TotalTicks(clock, 1.0 / fps + 1e-9, fps);
    CHECK(ticks == 60);
  }
}

/// @brief A fast frame can give no tick, and its time is kept.
void TestShortFrameKeepsItsTime() {
  FixedStepClock clock(kTick, kMaxTicks);
  CHECK(clock.Advance(0.010) == 0);
  CHECK(Near(clock.accumulator(), 0.010));
  // 10 ms + 10 ms is more than one 16.7 ms tick.
  CHECK(clock.Advance(0.010) == 1);
  CHECK(Near(clock.accumulator(), 0.020 - kTick));
}

/// @brief A slow frame gives several ticks.
void TestLongFrameGivesSeveralTicks() {
  FixedStepClock clock(kTick, kMaxTicks);
  CHECK(clock.Advance(4 * kTick + 0.001) == 4);
}

/// @brief Copies frames where each tick costs more real time than it
/// simulates, and returns the tick count of the last frame.
/// @param clock The clock to advance.
/// @param tick_cost Real seconds that one tick takes to run.
/// @param frames Number of frames.
/// @return Ticks in the last frame.
int TicksAfterSlowFrames(FixedStepClock& clock, double tick_cost, int frames) {
  double frame_seconds = kTick;
  int ticks = 0;
  for (int i = 0; i < frames; ++i) {
    ticks = clock.Advance(frame_seconds);
    // The next frame is as long as the ticks took, plus 1 ms to draw.
    frame_seconds = ticks * tick_cost + 0.001;
  }
  return ticks;
}

/// @brief Ticks that cost more than they simulate stay at the limit.
void TestSlowTicksStayAtTheLimit() {
  FixedStepClock clock(kTick, kMaxTicks);
  // 25 ms per 16.7 ms tick: without a limit, each frame would owe about 1.5
  // times the ticks of the frame before it. The limit stops that growth.
  CHECK(TicksAfterSlowFrames(clock, 0.025, 20) == kMaxTicks);
  CHECK(clock.dropped_seconds() > 0);
  CHECK(clock.accumulator() < kTick);
}

/// @brief A single long hitch drops time once, then the clock recovers.
void TestHitchDropsTimeOnce() {
  FixedStepClock clock(kTick, kMaxTicks);
  CHECK(clock.Advance(0.5) == kMaxTicks);
  const double dropped = clock.dropped_seconds();
  CHECK(Near(dropped, 0.5 - kMaxTicks * kTick - clock.accumulator()));
  // Normal frames after the hitch drop nothing more.
  CHECK(TotalTicks(clock, kTick, 60) >= 59);
  CHECK(clock.dropped_seconds() == dropped);
}

/// @brief Alpha is the leftover time as a fraction of a tick.
void TestAlpha() {
  FixedStepClock clock(kTick, kMaxTicks);
  CHECK(clock.Advance(kTick * 1.25) == 1);
  CHECK(Near(clock.Alpha(), 0.25));
}

/// @brief Reset() empties the accumulator.
void TestReset() {
  FixedStepClock clock(kTick, kMaxTicks);
  clock.Advance(0.010);
  clock.Reset();
  CHECK(clock.accumulator() == 0);
}

}  // namespace
}  // namespace ticker

/// @brief Runs every clock test.
/// @return 0 if all checks pass, otherwise 1.
int main() {
  using namespace ticker;
  const std::vector<testing::Test> tests = {
      {"sixty ticks per second at any frame rate",
       TestSixtyTicksPerSecondAtAnyFrameRate},
      {"short frame keeps its time", TestShortFrameKeepsItsTime},
      {"long frame gives several ticks", TestLongFrameGivesSeveralTicks},
      {"slow ticks stay at the limit", TestSlowTicksStayAtTheLimit},
      {"hitch drops time once", TestHitchDropsTimeOnce},
      {"alpha", TestAlpha},
      {"reset", TestReset},
  };
  return testing::RunTests(tests);
}
