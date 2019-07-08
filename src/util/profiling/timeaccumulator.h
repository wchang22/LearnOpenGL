#ifndef TIMEACCUMULATOR_H
#define TIMEACCUMULATOR_H

#include <vector>

namespace Profiling {
  class TimeAccumulator {
  public:
    ~TimeAccumulator();

    void add_time(double time);
    double get_average_time() const;

  private:
    std::vector<double> times;
  };
}

#endif // TIMEACCUMULATOR_H
