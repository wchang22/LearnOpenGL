#include "display.h"
#include "exception.h"
#include "data.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Display::Display(std::shared_ptr<Camera> camera)
  : shaders(nullptr), light_shaders(), camera(camera), textures()
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_shaders();
  init_buffers();
  init_textures();
}

Display::~Display() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteVertexArrays(1, &light_VAO);
}

void Display::draw() const {
  static const int num_indices = sizeof (indices) / sizeof(unsigned int);
  const double time = glfwGetTime();

  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();

  struct Light {
    vec3 color;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  };

  Light light {
    vec3(1.0f),
    vec3(1.2f, 1.0f, 2.0f),
    vec3(0.2f),
    vec3(0.5f),
    vec3(1.0f),
  };

  light.color.x = static_cast<float>(sin(time * 2.0));
  light.color.y = static_cast<float>(sin(time * 0.7));
  light.color.z = static_cast<float>(sin(time * 1.3));

  light.diffuse *= light.color;
  light.ambient *= light.diffuse;

  light.position.x = static_cast<float>(cos(time) + 1.0) * light.position.x;
  light.position.z = static_cast<float>(sin(time) + 1.0) * light.position.z;

  //-----------------------------------------------------------------------------------------------
  // Cube
  //-----------------------------------------------------------------------------------------------

  shaders->use_shader_program();
  glUniform3fv(shaders->get_uniform_location("light.position"), 1, &light.position[0]);
  glUniform3fv(shaders->get_uniform_location("light.ambient"), 1, &light.ambient[0]);
  glUniform3fv(shaders->get_uniform_location("light.diffuse"), 1, &light.diffuse[0]);
  glUniform3fv(shaders->get_uniform_location("light.specular"), 1, &light.specular[0]);

  glUniform1i(shaders->get_uniform_location("diffuse"), 0);
  glUniform1i(shaders->get_uniform_location("specular"), 1);
  glUniform1f(shaders->get_uniform_location("material.shininess"), 64.0f);

  glUniform3fv(shaders->get_uniform_location("view_position"), 1, &camera->get_position()[0]);

  mat4 model(1.0f);

  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glUniformMatrix4fv(shaders->get_uniform_location("perspective"),
                     1, GL_FALSE, &perspective[0][0]);
  glUniformMatrix4fv(shaders->get_uniform_location("view"),
                     1, GL_FALSE, &view[0][0]);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, buffer_offset(0));

  //-----------------------------------------------------------------------------------------------
  // Light
  //-----------------------------------------------------------------------------------------------

  light_shaders->use_shader_program();
  model = glm::translate(mat4(1.0f), light.position);
  model = glm::scale(model, vec3(0.2f));

  glUniformMatrix4fv(light_shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glUniformMatrix4fv(light_shaders->get_uniform_location("perspective"),
                     1, GL_FALSE, &perspective[0][0]);
  glUniformMatrix4fv(light_shaders->get_uniform_location("view"),
                     1, GL_FALSE, &view[0][0]);

  glUniform3fv(light_shaders->get_uniform_location("light_color"), 1, &light.color[0]);

  glBindVertexArray(light_VAO);
  glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, buffer_offset(0));
}

void Display::init_buffers() {
  //-----------------------------------------------------------------------------------------------
  // Cube
  //-----------------------------------------------------------------------------------------------

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  const int position_size = 3;
  const int normal_size = 3;
  const int texture_size = 2;
  const int vertex_stride = (position_size + normal_size + texture_size) * sizeof (float);

  const unsigned int position_location = 0;
  const void* position_offset = buffer_offset(0);
  glVertexAttribPointer(position_location, position_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, position_offset);
  glEnableVertexAttribArray(position_location);

  const unsigned int normal_location = 1;
  const void* normal_offset = buffer_offset(position_size * sizeof (float));
  glVertexAttribPointer(normal_location, normal_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, normal_offset);
  glEnableVertexAttribArray(normal_location);

  const unsigned int texture_location = 2;
  const void* texture_offset = buffer_offset((position_size + normal_size) * sizeof (float));
  glVertexAttribPointer(texture_location, texture_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, texture_offset);
  glEnableVertexAttribArray(texture_location);

  //-----------------------------------------------------------------------------------------------
  // Light
  //-----------------------------------------------------------------------------------------------

  glGenVertexArrays(1, &light_VAO);

  glBindVertexArray(light_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glVertexAttribPointer(position_location, position_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, position_offset);
  glEnableVertexAttribArray(position_location);

  //-----------------------------------------------------------------------------------------------
  // Unbind
  //-----------------------------------------------------------------------------------------------

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Display::init_textures() {
  textures.load_texture_from_image("../../assets/container2.png");
  textures.load_texture_from_image("../../assets/container2_specular.png");
  textures.use_textures();
}

void Display::init_shaders() {
  shaders = std::make_unique<Shader>("../../shaders/cube_vertex.glsl",
                                     "../../shaders/cube_fragment.glsl");
  light_shaders = std::make_unique<Shader>("../../shaders/light_vertex.glsl",
                                           "../../shaders/light_fragment.glsl");
}

void* Display::buffer_offset(int offset) {
  return reinterpret_cast<void*>(offset);
}
