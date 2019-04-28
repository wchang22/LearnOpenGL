#include "window.h"
#include "exception.h"

const int WIDTH = 800;
const int HEIGHT = 600;

Window::Window()
  : window(nullptr), display(nullptr)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

  if (!window) {
    glfwDestroyWindow(window);
    glfwTerminate();
    throw Exception::WindowException("Failed to create GLFW Window");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwDestroyWindow(window);
    glfwTerminate();
    throw Exception::WindowException("Failed to create initialize GLAD");
  }

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);

  display = std::make_unique<Display>();
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::main_loop() {
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    display->draw();

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
    case GLFW_KEY_T:
      cycle_fill_mode();
      break;
    default:
      break;
  }
}

void Window::cycle_fill_mode() {
  static GLenum fill_mode = GL_FILL;

  switch (fill_mode) {
    case GL_FILL:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      fill_mode = GL_LINE;
      break;
    case GL_LINE:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      fill_mode = GL_FILL;
      break;
  }
}
