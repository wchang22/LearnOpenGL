#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string_view>

#define GENERATE_EXCEPTION_HEADER(name)                                        \
  struct name : public std::runtime_error {                                    \
    name(std::string_view msg) : std::runtime_error(msg.data()) {}             \
    const char* what() const noexcept override;                                \
  };                                                                           \

#define GENERATE_EXCEPTION_IMPL(name)                                          \
  const char* name::what() const noexcept {                                    \
    return std::runtime_error::what();                                         \
  }                                                                            \

GENERATE_EXCEPTION_HEADER(ShaderException)
GENERATE_EXCEPTION_HEADER(WindowException)
GENERATE_EXCEPTION_HEADER(DisplayException)
GENERATE_EXCEPTION_HEADER(TextureException)
GENERATE_EXCEPTION_HEADER(MeshException)
GENERATE_EXCEPTION_HEADER(ModelException)
GENERATE_EXCEPTION_HEADER(ShadowException)
GENERATE_EXCEPTION_HEADER(FrameBufferException)

#endif // EXCEPTION_H
