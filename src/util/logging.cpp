#include "logging.h"

#include "util/exception.h"

#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>

using namespace std::chrono;

logger_t Logging::logger = nullptr;

logger_t Logging::get_logger()
{
  if (logger) {
    return logger;
  }

#ifdef LOG
  std::filesystem::create_directory("logs");

  auto log_time = system_clock::to_time_t(system_clock::now());
  auto log_local_time = std::localtime(&log_time);
  std::stringstream ss;
  ss << std::put_time(log_local_time, "logs/%Y-%m-%j-%T.log");
  logger = std::make_shared<std::ofstream>(ss.str());

  if (!logger->is_open()) {
    throw LoggingException("Cannot open logging file " + ss.str());
  }
#endif

  return logger;
}
