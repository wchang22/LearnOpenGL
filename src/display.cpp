#include "display.h"
#include "exception.h"
#include "data.h"
#include "window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>

const vec3 dir_light_dir = vec3(2.0f, -4.0f, 1.0f);

Display::Display(std::shared_ptr<Camera> camera)
  : camera(camera),
    model_nanosuit("../../assets/nanosuit_reflection/nanosuit.obj"),
    model_aircraft("../../assets/aircraft/aircraft.obj"),
    shadow(1024, 1024, Window::width(), Window::height(), dir_light_dir)
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
  glDeleteBuffers(1, &UBO);
  glDeleteBuffers(1, &lightsUBO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &skyboxVAO);
}

void Display::draw() const {
  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();
  mat4 world_space = perspective * view;
  mat4 model(1.0f);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), &world_space[0][0]);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), &model[0][0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  shadow.bind_depth_map();

  draw_model(*dir_depth_shaders);
  draw_cubes(*dir_depth_shaders);
  draw_floor(*dir_depth_shaders);

  shadow.bind_shadow_map("shadow_map", { shaders, model_shaders });
  set_lights();

  draw_cubes(*shaders);
  draw_floor(*shaders);
  draw_model(*model_shaders);
  draw_skybox();
}

void Display::init_buffers() {
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);

  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);


  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), buffer_offset(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), buffer_offset(6 * sizeof(float)));
  glEnableVertexAttribArray(2);


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


  glGenBuffers(1, &lightsUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
  glBufferData(GL_UNIFORM_BUFFER, (5 + NUM_POINT_LIGHTS * 5) * sizeof(vec4),
               nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);


  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::init_textures() {
  textures.load_texture_from_image("../../assets/wood.png", "texture_diffuse");
  skybox_textures.load_cubemap({
    "../../assets/space/right.jpg",
    "../../assets/space/left.jpg",
    "../../assets/space/top.jpg",
    "../../assets/space/bottom.jpg",
    "../../assets/space/front.jpg",
    "../../assets/space/back.jpg",
  });

  for (auto it = model_nanosuit.meshes.begin(); it != model_nanosuit.meshes.end(); it++) {
    it->textures.append(skybox_textures);
  }
  for (auto it = model_aircraft.meshes.begin(); it != model_aircraft.meshes.end(); it++) {
    it->textures.append(skybox_textures);
  }
}

void Display::init_shaders() {
  shaders = std::make_unique<Shader>("../../shaders/cube_vertex.glsl",
                                     "../../shaders/cube_fragment.glsl");
  model_shaders = std::make_unique<Shader>("../../shaders/model_vertex.glsl",
                                           "../../shaders/model_fragment.glsl");
  skybox_shaders = std::make_unique<Shader>("../../shaders/skybox_vertex.glsl",
                                            "../../shaders/skybox_fragment.glsl");
  dir_depth_shaders = std::make_unique<Shader>("../../shaders/dir_depth_vertex.glsl",
                                           "../../shaders/dir_depth_fragment.glsl");
}

void Display::set_lights() const
{
  struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  };

  static DirLight dir_light {
    dir_light_dir,
    vec3(0.05f),
    vec3(0.4f),
    vec3(0.5f),
  };

  struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
  };

  static PointLight point_light {
    vec3(0.05f),
    vec3(0.8f),
    vec3(1.0f),
    vec3(1.0f, 0.09f, 0.032f),
  };

  glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);

  float shininess = 64.0f;

  glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof (vec4), sizeof (vec3), &dir_light.direction[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (vec4), sizeof (vec3), &dir_light.ambient[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof (vec4), sizeof (vec3), &dir_light.diffuse[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof (vec4), sizeof (vec3), &dir_light.specular[0]);

  for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
    glBufferSubData(GL_UNIFORM_BUFFER, (4 + i * 5) * sizeof (vec4), sizeof (vec3), &POINT_LIGHT_POSITIONS[i][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, (5 + i * 5) * sizeof (vec4), sizeof (vec3), &point_light.ambient[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, (6 + i * 5) * sizeof (vec4), sizeof (vec3), &point_light.diffuse[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, (7 + i * 5) * sizeof (vec4), sizeof (vec3), &point_light.specular[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, (8 + i * 5) * sizeof (vec4), sizeof (vec3), &point_light.attenuation[0]);
  }

  glBufferSubData(GL_UNIFORM_BUFFER, (4 + NUM_POINT_LIGHTS * 5) * sizeof (vec4),
                  sizeof (vec3), &camera->get_position()[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, (4 + NUM_POINT_LIGHTS * 5) * sizeof (vec4) + sizeof (vec3),
                  sizeof (float), &shininess);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::draw_cubes(const Shader& shader) const
{
  shader.use_shader_program();

  glBindVertexArray(cubeVAO);
  textures.use_textures(shader);
  mat4 model(1.0f);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  model = glm::translate(model, vec3(-1.0f, 0.0f, -1.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  model = glm::translate(mat4(1.0f), vec3(2.0f, 0.0f, 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::draw_floor(const Shader& shader) const
{
  shader.use_shader_program();

  glBindVertexArray(planeVAO);
  textures.use_textures(shader);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  mat4 model(1.0f);
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::draw_model(const Shader& shader) const
{
  shader.use_shader_program();

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  mat4 model = glm::scale(mat4(1.0f), vec3(0.2f, 0.2, 0.2f));
  model = glm::rotate(model, -static_cast<float>(glfwGetTime()), vec3(0, 1, 0));
  model = glm::translate(model, vec3(0.0f, -2.5f, 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  model_nanosuit.draw(shader);

  model = glm::scale(mat4(1.0f), vec3(0.6f));
  model = glm::rotate(model, static_cast<float>(glfwGetTime()), vec3(0, 1, 0));
  model = glm::translate(model, vec3(5.0f, 2.0f, 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  model_aircraft.draw(shader);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::draw_skybox() const
{
  glDepthFunc(GL_LEQUAL);

  mat4 view = mat4(mat3(camera->lookat()));
  mat4 perspective = camera->perspective();
  mat4 world_space = perspective * view;

  skybox_shaders->use_shader_program();
  skybox_textures.use_textures(*skybox_shaders);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), &world_space[0][0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindVertexArray(skyboxVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glDepthFunc(GL_LESS);
}

void* Display::buffer_offset(int offset) {
  return reinterpret_cast<void*>(offset);
}
