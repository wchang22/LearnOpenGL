#include "exception.h"

Exception::WindowException::WindowException(const char* msg) : std::runtime_error(msg) {}

const char* Exception::WindowException::what() const noexcept {
  return std::runtime_error::what();
}

Exception::ShaderException::ShaderException(const char* msg) : std::runtime_error(msg) {}

const char* Exception::ShaderException::what() const noexcept {
  return std::runtime_error::what();
}

Exception::DisplayException::DisplayException(const char* msg) : std::runtime_error(msg) {}

const char* Exception::DisplayException::what() const noexcept {
  return std::runtime_error::what();
}
