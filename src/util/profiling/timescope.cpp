#include "timescope.h"
#include "util/logging.h"
#include "util/profiling/timeaccumulator.h"

namespace Profiling {
  using namespace std::chrono;
  static auto logger = Logging::get_logger();
  static TimeAccumulator accumulator;

  TimeScope::TimeScope(std::string_view name, bool fps)
    : start(steady_clock::now()),
      name(name),
      fps(fps)
  {
  }

  TimeScope::~TimeScope() {
    const auto duration = duration_cast<microseconds>(steady_clock::now() - start).count();
    logger << "" << name << " total time: " << duration << " us";

    if (fps) {
      logger << "\nFPS (" << name << "): " << (1.0 / duration * 1e6) << std::endl;
    }

    logger << "\n" << std::endl;

    accumulator.add_time(duration);
  }

  void TimeScope::section_start(std::optional<std::string_view> message)
  {
    this->message = message;
    t0 = steady_clock::now();
  }

  void TimeScope::section_end()
  {
    auto duration = duration_cast<microseconds>(steady_clock::now() - t0).count();
    logger << name;

    if (message.has_value()) {
      logger << " (" << message.value() << ")";
    }

    logger << " time: " << duration << " us" << std::endl;
  }
}
