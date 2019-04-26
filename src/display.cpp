#include "display.h"

#include <glad/glad.h>

const float vertices[] = {
  -0.5f, -0.5f, 0.0f,
   0.5f, -0.5f, 0.0f,
   0.0f,  0.5f, 0.0f,
};

Display::Display() {
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  const unsigned int location = 0;
  const int vertex_size = 3;
  const void* offset = static_cast<const void*>(0);
  glVertexAttribPointer(location, vertex_size, GL_FLOAT, GL_FALSE,
                        vertex_size * sizeof(float), offset);
  glEnableVertexAttribArray(location);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Display::draw() const {
  const int starting_vertex = 0;
  const int num_vertices = 3;

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, starting_vertex, num_vertices);
}
