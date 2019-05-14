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

Exception::TextureException::TextureException(const char* msg) : std::runtime_error(msg) {}

const char* Exception::TextureException::what() const noexcept {
  return std::runtime_error::what();
}

Exception::MeshException::MeshException(const char* msg) : std::runtime_error(msg) {}

const char* Exception::MeshException::what() const noexcept {
  return std::runtime_error::what();
}

Exception::ModelException::ModelException(const char* msg) : std::runtime_error(msg) {}

const char* Exception::ModelException::what() const noexcept {
  return std::runtime_error::what();
}
