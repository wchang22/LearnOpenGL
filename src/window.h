#ifndef WINDOW_H
#define WINDOW_H

#include <stdexcept>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

class Window {
public:
  Window();
  ~Window();

  void main_loop() const;

  struct WindowException : public std::runtime_error {
    WindowException(const char* msg);
    const char* what() const noexcept override;
  };

private:
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

  GLFWwindow* window;
  std::unique_ptr<Shader> shaders;
};

#endif // WINDOW_H
