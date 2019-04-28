#ifndef WINDOW_H
#define WINDOW_H

#include "display.h"

#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
  Window();
  ~Window();

  void main_loop();

private:
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cycle_fill_mode();

  GLFWwindow* window;
  std::unique_ptr<Display> display;
};

#endif // WINDOW_H
