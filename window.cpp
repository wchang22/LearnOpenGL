#include "window.h"

const int WIDTH = 800;
const int HEIGHT = 600;

Window::Window() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    throw WindowException("Failed to create GLFW Window");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwTerminate();
    throw WindowException("Failed to create initialize GLAD");
  }

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);
}

Window::~Window() {
  glfwTerminate();
}

void Window::main_loop() {
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void) window;
  glViewport(0, 0, width, height);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void) scancode;
  (void) mods;

  if (action != GLFW_PRESS) {
    return;
  }

  switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, true);
      break;
    default:
      break;
  }
}

Window::WindowException::WindowException(const char* msg) : std::runtime_error(msg) {}

const char* Window::WindowException::what() const noexcept {
  return std::runtime_error::what();
}
