#ifndef WINDOW_H
#define WINDOW_H

#include "display.h"
#include "camera.h"

#include <memory>

#include <GLFW/glfw3.h>

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;

class Window {
public:
  Window();
  ~Window();

  void main_loop();

  static int width();
  static int height();

private:
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void scroll_callback(GLFWwindow* window, double delta_x, double delta_y);
  void key_callback();
  void mouse_callback();
  static void cycle_fill_mode();

  GLFWwindow* window;
  std::unique_ptr<Display> display;
  std::shared_ptr<Camera> camera;
};

#endif // WINDOW_H
