#include "timeaccumulator.h"
#include "util/logging.h"

#include <numeric>

namespace Profiling {
  static auto logger = Logging::get_logger();

  TimeAccumulator::~TimeAccumulator()
  {
    logger << "Average time: " << get_average_time() << " us" << std::endl;
  }

  void TimeAccumulator::add_time(double time)
  {
    times.emplace_back(time);
  }

  double TimeAccumulator::get_average_time() const
  {
    return std::accumulate(times.begin(), times.end(), 0.0, [this] (double a, double b) {
      return (a * times.size() + b) / times.size();
    });
  }
}

