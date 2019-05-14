#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

class Exception
{
public:

  struct ShaderException : public std::runtime_error {
    ShaderException(const char* msg);
    const char* what() const noexcept override;
  };

  struct WindowException : public std::runtime_error {
    WindowException(const char* msg);
    const char* what() const noexcept override;
  };

  struct DisplayException : public std::runtime_error {
    DisplayException(const char* msg);
    const char* what() const noexcept override;
  };

  struct TextureException : public std::runtime_error {
    TextureException(const char* msg);
    const char* what() const noexcept override;
  };

  struct MeshException : public std::runtime_error {
    MeshException(const char* msg);
    const char* what() const noexcept override;
  };

  struct ModelException : public std::runtime_error {
    ModelException(const char* msg);
    const char* what() const noexcept override;
  };
};

#endif // EXCEPTION_H
