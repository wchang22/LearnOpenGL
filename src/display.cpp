#include "display.h"
#include "exception.h"
#include "data.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Display::Display(std::shared_ptr<Camera> camera)
  : shaders(nullptr), camera(camera), textures()
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_shaders();
  init_textures();
  init_buffers();

  glBindVertexArray(VAO);
}

Display::~Display() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
}

void Display::draw() const {
  static const int num_indices = sizeof (indices) / sizeof(unsigned int);
  static const int model_location = shaders->get_uniform_location("model");
  const double time = glfwGetTime();

  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();

  glUniformMatrix4fv(shaders->get_uniform_location("perspective"),
                     1, GL_FALSE, &perspective[0][0]);
  glUniformMatrix4fv(shaders->get_uniform_location("view"),
                     1, GL_FALSE, &view[0][0]);

  for (int i = 0; i < NUM_CUBES; i++) {
    mat4 rot = glm::rotate(mat4(1.0f), glm::radians(20.0f * i) + static_cast<float>(time),
                         vec3(1.0f, 0.3f, 0.5f));
    mat4 model = glm::translate(mat4(1.0f), cubePositions[i]) * rot;

    glUniformMatrix4fv(model_location, 1, GL_FALSE, &model[0][0]);

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, buffer_offset(0));
  }
}

void Display::init_buffers() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  const int position_size = 3;
  const int texture_size = 2;
  const int vertex_stride = (position_size + texture_size) * sizeof (float);

  const unsigned int position_location = 0;
  const void* position_offset = buffer_offset(0);
  glVertexAttribPointer(position_location, position_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, position_offset);
  glEnableVertexAttribArray(position_location);

  const unsigned int texture_location = 2;
  const void* texture_offset = buffer_offset((position_size) * sizeof(float));
  glVertexAttribPointer(texture_location, texture_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, texture_offset);
  glEnableVertexAttribArray(texture_location);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Display::init_textures() {
  const char* texture_path_0 = "../../assets/container.jpg";
  const char* texture_path_1 = "../../assets/awesomeface.png";

  textures.load_texture_from_image(texture_path_0, GL_RGB);
  textures.load_texture_from_image(texture_path_1, GL_RGBA);
  textures.use_textures();

  glUniform1i(shaders->get_uniform_location("texture0"), 0);
  glUniform1i(shaders->get_uniform_location("texture1"), 1);
}

void Display::init_shaders() {
  shaders = std::make_unique<Shader>("../../shaders/vertex.glsl", "../../shaders/fragment.glsl");
  shaders->use_shader_program();
}

void* Display::buffer_offset(int offset) {
  return reinterpret_cast<void*>(offset);
}
