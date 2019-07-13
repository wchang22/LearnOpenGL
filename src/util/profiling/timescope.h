#ifndef TIMESCOPE_H
#define TIMESCOPE_H

#include <chrono>
#include <optional>

namespace Profiling {
  class TimeScope {
  public:
    TimeScope(std::string_view name, bool fps = false);
    ~TimeScope();

    void section_start(std::optional<std::string_view> message = std::nullopt);
    void section_end();

  private:
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point t0;
    std::optional<std::string_view> message;
    std::string_view name;
    bool fps;
  };
}

#endif // TIMESCOPE_H
