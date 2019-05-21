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
    skybox_shaders(nullptr),
    camera(camera),
    textures(),
    model_nanosuit("../../assets/nanosuit/nanosuit.obj"),
    model_aircraft("../../assets/aircraft/aircraft.obj")
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_shaders();
  init_buffers();
  init_textures();
}

Display::~Display() {
  glDeleteBuffers(1, &planeVBO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &skyboxVBO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &skyboxVAO);
}

void Display::draw() const {
  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();

  shaders->use_shader_program();
  glUniformMatrix4fv(shaders->get_uniform_location("view"), 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(shaders->get_uniform_location("perspective"), 1, GL_FALSE, &perspective[0][0]);

  draw_model();
  draw_cubes();
  draw_floor();
  draw_skybox();
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), buffer_offset(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), buffer_offset(6 * sizeof(float)));
  glEnableVertexAttribArray(2);


  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);

  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Display::init_textures() {
  textures.load_texture_from_image("../../assets/metal.jpg", "texture_diffuse");
  textures.load_cubemap({
    "../../assets/skybox/right.jpg",
    "../../assets/skybox/left.jpg",
    "../../assets/skybox/top.jpg",
    "../../assets/skybox/bottom.jpg",
    "../../assets/skybox/front.jpg",
    "../../assets/skybox/back.jpg"
  });
}

void Display::init_shaders() {
  shaders = std::make_unique<Shader>("../../shaders/cube_vertex.glsl",
                                     "../../shaders/cube_fragment.glsl");
  skybox_shaders = std::make_unique<Shader>("../../shaders/skybox_vertex.glsl",
                                            "../../shaders/skybox_fragment.glsl");
}

void Display::draw_cubes() const
{
  glBindVertexArray(cubeVAO);
  textures.use_textures(*shaders);
  mat4 model(1.0f);

  model = glm::translate(model, vec3(-1.0f, 0.0f, -1.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  model = glm::translate(mat4(1.0f), vec3(2.0f, 0.0f, 0.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);

  glUniform3fv(shaders->get_uniform_location("cameraPos"), 1, &camera->get_position()[0]);

  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Display::draw_floor() const
{
  glBindVertexArray(planeVAO);
  textures.use_textures(*shaders);

  mat4 model(1.0f);
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Display::draw_model() const
{
  float time = static_cast<float>(glfwGetTime());
  mat4 model_mat = glm::scale(mat4(1.0f), vec3(0.2f, 0.2, 0.2f));
  model_mat = glm::translate(model_mat, vec3(0.0f, 8.0f, 0.0f));
  model_mat = glm::rotate(model_mat, time, vec3(0.0f, 1.0f, 0.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model_mat[0][0]);
  model_nanosuit.draw(*shaders);

  model_mat = glm::scale(mat4(1.0f), vec3(0.8f, 0.8, 0.8f));
  model_mat = glm::translate(model_mat, vec3(0.0f, 2.0f, 0.0f));
  model_mat = glm::rotate(model_mat, glm::radians(180.0f) + time, vec3(0.0f, 1.0f, 0.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model_mat[0][0]);
  model_aircraft.draw(*shaders);
}

void Display::draw_skybox() const
{
  glDepthFunc(GL_LEQUAL);

  mat4 view = mat4(mat3(camera->lookat()));
  mat4 perspective = camera->perspective();

  skybox_shaders->use_shader_program();
  glUniformMatrix4fv(skybox_shaders->get_uniform_location("view"), 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(skybox_shaders->get_uniform_location("perspective"), 1, GL_FALSE, &perspective[0][0]);

  textures.use_textures(*skybox_shaders);

  glBindVertexArray(skyboxVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glDepthFunc(GL_LESS);
}

void* Display::buffer_offset(int offset) {
  return reinterpret_cast<void*>(offset);
}
