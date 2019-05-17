#include "display.h"
#include "exception.h"
#include "data.h"

#include <map>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>

Display::Display(std::shared_ptr<Camera> camera)
  : shaders(nullptr),
    camera(camera),
    metal_texture(),
    marble_texture(),
    transparent_texture()
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_shaders();
  init_buffers();
  init_textures();
}

Display::~Display() {
  glDeleteBuffers(1, &planeVBO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteVertexArrays(1, &cubeVAO);
}

void Display::draw() const {
  shaders->use_shader_program();

  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();

  glUniformMatrix4fv(shaders->get_uniform_location("view"), 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(shaders->get_uniform_location("perspective"), 1, GL_FALSE, &perspective[0][0]);

  draw_cubes();
  draw_floor();
  draw_transparent();
}

void Display::init_buffers() {
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);

  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), buffer_offset(3 * sizeof(float)));
  glEnableVertexAttribArray(1);


  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);

  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), buffer_offset(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);

  glBindVertexArray(transparentVAO);
  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), buffer_offset(3 * sizeof(float)));
  glEnableVertexAttribArray(1);


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Display::init_textures() {
  marble_texture.load_texture_from_image("../../assets/marble.jpg", "texture_diffuse");
  metal_texture.load_texture_from_image("../../assets/metal.jpg", "texture_diffuse");
  transparent_texture.load_texture_from_image("../../assets/blending_transparent_window.png", "texture_diffuse");
}

void Display::init_shaders() {
  shaders = std::make_unique<Shader>("../../shaders/cube_vertex.glsl",
                                     "../../shaders/cube_fragment.glsl");
}

void Display::draw_cubes() const
{
  glBindVertexArray(cubeVAO);
  metal_texture.use_textures(*shaders);
  mat4 model(1.0f);

  model = glm::translate(model, vec3(-1.0f, 0.0f, -1.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  model = glm::translate(mat4(1.0f), vec3(2.0f, 0.0f, 0.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Display::draw_floor() const
{
  glBindVertexArray(planeVAO);
  marble_texture.use_textures(*shaders);

  mat4 model(1.0f);
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Display::draw_transparent() const
{
  std::map<float, vec3, std::greater<float>> positions;

  for (const vec3& position: transparent) {
    float distance = glm::distance(camera->get_position(), position);
    positions[distance] = position;
  }

  glBindVertexArray(transparentVAO);
  transparent_texture.use_textures(*shaders);
  for (auto& it : positions) {
    mat4 model = glm::translate(mat4(1.0f), it.second);
    glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void* Display::buffer_offset(int offset) {
  return reinterpret_cast<void*>(offset);
}
