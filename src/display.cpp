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
    model_shaders(nullptr),
    camera(camera),
    textures(),
    model_nanosuit("../../assets/nanosuit_reflection/nanosuit.obj")
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
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &skyboxVAO);
}

void Display::draw() const {
  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), &view[0][0]);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), &perspective[0][0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

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


  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), nullptr, GL_STATIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof (mat4));


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::init_textures() {
  textures.load_cubemap({
    "../../assets/skybox/right.jpg",
    "../../assets/skybox/left.jpg",
    "../../assets/skybox/top.jpg",
    "../../assets/skybox/bottom.jpg",
    "../../assets/skybox/front.jpg",
    "../../assets/skybox/back.jpg"
  });

  for (auto it = model_nanosuit.meshes.begin(); it != model_nanosuit.meshes.end(); it++) {
    it->textures.append(textures);
  }
}

void Display::init_shaders() {
  shaders = std::make_unique<Shader>("../../shaders/cube_vertex.glsl",
                                     "../../shaders/cube_fragment.glsl");
  model_shaders = std::make_unique<Shader>("../../shaders/model_vertex.glsl",
                                           "../../shaders/model_fragment.glsl");
  skybox_shaders = std::make_unique<Shader>("../../shaders/skybox_vertex.glsl",
                                            "../../shaders/skybox_fragment.glsl");
}

void Display::draw_cubes() const
{
  shaders->use_shader_program();

  glBindVertexArray(cubeVAO);
  textures.use_textures(*shaders);
  mat4 model(1.0f);

  model = glm::translate(model, vec3(-1.0f, 0.0f, -1.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  model = glm::translate(mat4(1.0f), vec3(2.0f, 0.0f, 0.0f));
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);

  glUniform3fv(shaders->get_uniform_location("view_position"), 1, &camera->get_position()[0]);

  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Display::draw_floor() const
{
  shaders->use_shader_program();

  glBindVertexArray(planeVAO);
  textures.use_textures(*shaders);

  mat4 model(1.0f);
  glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Display::draw_model() const
{
  model_shaders->use_shader_program();

  struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  };

  static DirLight dir_light {
    vec3(-0.2f, -1.0f, -0.3f),
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

  glUniform3fv(model_shaders->get_uniform_location("view_position"), 1, &camera->get_position()[0]);

  glUniform3fv(model_shaders->get_uniform_location("dir_light.direction"), 1, &dir_light.direction[0]);
  glUniform3fv(model_shaders->get_uniform_location("dir_light.ambient"), 1, &dir_light.ambient[0]);
  glUniform3fv(model_shaders->get_uniform_location("dir_light.diffuse"), 1, &dir_light.diffuse[0]);
  glUniform3fv(model_shaders->get_uniform_location("dir_light.specular"), 1, &dir_light.specular[0]);

  for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
    std::string point_light_str = "point_light[" + std::to_string(i) + "].";
    std::string position_str = point_light_str + "position";
    std::string ambient_str = point_light_str + "ambient";
    std::string diffuse_str = point_light_str + "diffuse";
    std::string specular_str = point_light_str + "specular";
    std::string attenuation_str = point_light_str + "attenuation";

    glUniform3fv(model_shaders->get_uniform_location(position_str.c_str()), 1, &POINT_LIGHT_POSITIONS[i][0]);
    glUniform3fv(model_shaders->get_uniform_location(ambient_str.c_str()), 1, &point_light.ambient[0]);
    glUniform3fv(model_shaders->get_uniform_location(diffuse_str.c_str()), 1, &point_light.diffuse[0]);
    glUniform3fv(model_shaders->get_uniform_location(specular_str.c_str()), 1, &point_light.specular[0]);
    glUniform3fv(model_shaders->get_uniform_location(attenuation_str.c_str()), 1, &point_light.attenuation[0]);
  }

  glUniform1f(model_shaders->get_uniform_location("material_shininess"), 64.0f);

  float time = static_cast<float>(glfwGetTime());
  mat4 model_mat = glm::scale(mat4(1.0f), vec3(0.2f, 0.2, 0.2f));
  model_mat = glm::translate(model_mat, vec3(0.0f, -2.5f, 0.0f));
  model_mat = glm::rotate(model_mat, time, vec3(0.0f, 1.0f, 0.0f));
  glUniformMatrix4fv(model_shaders->get_uniform_location("model"), 1, GL_FALSE, &model_mat[0][0]);
  model_nanosuit.draw(*model_shaders);
}

void Display::draw_skybox() const
{
  glDepthFunc(GL_LEQUAL);

  mat4 view = mat4(mat3(camera->lookat()));

  skybox_shaders->use_shader_program();
  textures.use_textures(*skybox_shaders);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), &view[0][0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindVertexArray(skyboxVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glDepthFunc(GL_LESS);
}

void* Display::buffer_offset(int offset) {
  return reinterpret_cast<void*>(offset);
}
