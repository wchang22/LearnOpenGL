#include "window.h"
#include "shader.h"

#include <iostream>

int main() {
  try {
    Window window;
    window.main_loop();
  } catch (Window::WindowException e) {
    std::cerr << e.what() << std::endl;
    return -1;
  } catch (Shader::ShaderException e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
