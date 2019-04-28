#include "display.h"

#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const float vertices[] = {
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
};
const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

Display::Display()
  : shaders(std::make_unique<Shader>("../shaders/vertex.glsl", "../shaders/fragment.glsl"))
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  const int num_vertices = 3;
  const int vertex_stride = 6 * sizeof (float);

  const unsigned int position_location = 0;
  const void* position_offset = buffer_offset(0);
  glVertexAttribPointer(position_location, num_vertices, GL_FLOAT, GL_FALSE,
                        vertex_stride, position_offset);
  glEnableVertexAttribArray(position_location);

  const unsigned int color_location = 1;
  const void* color_offset = buffer_offset(3 * sizeof(float));
  glVertexAttribPointer(color_location, num_vertices, GL_FLOAT, GL_FALSE,
                        vertex_stride, color_offset);
  glEnableVertexAttribArray(color_location);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  shaders->use_shader_program();
}

Display::~Display() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
}

void Display::draw() const {
  const int num_vertices = 6;

  const double time = glfwGetTime();
  const float color_offset = static_cast<float>(sin(time) / 2.0);
  const int color_location = shaders->get_uniform_location("color");
  glUniform4f(color_location, color_offset, color_offset, color_offset, 1.0);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, buffer_offset(0));
}

void *Display::buffer_offset(int offset) {
  return static_cast<void*>(static_cast<char*>(nullptr) + offset);
}
