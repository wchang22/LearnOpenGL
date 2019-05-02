#include "window.h"
#include "exception.h"

Window::Window()
  : window(nullptr), display(nullptr),
    camera(std::make_shared<Camera>(vec3(0.0f, 0.0f, 3.0f),
                                    vec3(0.0f, 0.0f, -1.0f),
                                    vec3(0.0f, 1.0f, 0.0f)))
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

  display = std::make_unique<Display>(camera);
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::main_loop() {
  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->update_frames();
    display->draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
    key_callback(window);
  }
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void) window;
  glViewport(0, 0, width, height);
}

void Window::key_callback(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    cycle_fill_mode();
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera->move_forward();
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera->move_backward();
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera->move_left();
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera->move_right();
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
