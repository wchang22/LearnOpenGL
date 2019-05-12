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
  const double time = glfwGetTime();

  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();

  //-----------------------------------------------------------------------------------------------
  // Lighting
  //-----------------------------------------------------------------------------------------------

  struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
    float inner_cutoff;
    float outer_cutoff;
  };

  static SpotLight spotlight {
    vec3(0.0f),
    vec3(1.0f),
    vec3(1.0f),
    vec3(1.0f, 0.09f, 0.032f),
    glm::cos(glm::radians(12.5f)),
    glm::cos(glm::radians(15.0f))
  };

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

  shaders->use_shader_program();

  glUniform3fv(shaders->get_uniform_location("view_position"), 1, &camera->get_position()[0]);

  glUniform3fv(shaders->get_uniform_location("dir_light.direction"), 1, &dir_light.direction[0]);
  glUniform3fv(shaders->get_uniform_location("dir_light.ambient"), 1, &dir_light.ambient[0]);
  glUniform3fv(shaders->get_uniform_location("dir_light.diffuse"), 1, &dir_light.diffuse[0]);
  glUniform3fv(shaders->get_uniform_location("dir_light.specular"), 1, &dir_light.specular[0]);

  for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
    std::string point_light_str = "point_light[" + std::to_string(i) + "].";
    std::string position_str = point_light_str + "position";
    std::string ambient_str = point_light_str + "ambient";
    std::string diffuse_str = point_light_str + "diffuse";
    std::string specular_str = point_light_str + "specular";
    std::string attenuation_str = point_light_str + "attenuation";

    glUniform3fv(shaders->get_uniform_location(position_str.c_str()), 1, &POINT_LIGHT_POSITIONS[i][0]);
    glUniform3fv(shaders->get_uniform_location(ambient_str.c_str()), 1, &point_light.ambient[0]);
    glUniform3fv(shaders->get_uniform_location(diffuse_str.c_str()), 1, &point_light.diffuse[0]);
    glUniform3fv(shaders->get_uniform_location(specular_str.c_str()), 1, &point_light.specular[0]);
    glUniform3fv(shaders->get_uniform_location(attenuation_str.c_str()), 1, &point_light.attenuation[0]);
  }

  glUniform3fv(shaders->get_uniform_location("spotlight.position"), 1, &camera->get_position()[0]);
  glUniform3fv(shaders->get_uniform_location("spotlight.direction"), 1, &camera->get_direction()[0]);
  glUniform1f(shaders->get_uniform_location("spotlight.inner_cutoff"), spotlight.inner_cutoff);
  glUniform1f(shaders->get_uniform_location("spotlight.outer_cutoff"), spotlight.outer_cutoff);
  glUniform3fv(shaders->get_uniform_location("spotlight.ambient"), 1, &spotlight.ambient[0]);
  glUniform3fv(shaders->get_uniform_location("spotlight.diffuse"), 1, &spotlight.diffuse[0]);
  glUniform3fv(shaders->get_uniform_location("spotlight.specular"), 1, &spotlight.specular[0]);
  glUniform3fv(shaders->get_uniform_location("spotlight.attenuation"), 1, &spotlight.attenuation[0]);

  //-----------------------------------------------------------------------------------------------
  // Draw Cubes
  //-----------------------------------------------------------------------------------------------

  glUniform1i(shaders->get_uniform_location("material_diffuse"), 0);
  glUniform1i(shaders->get_uniform_location("material_specular"), 1);
  glUniform1f(shaders->get_uniform_location("material_shininess"), 64.0f);

  glUniformMatrix4fv(shaders->get_uniform_location("perspective"),
                     1, GL_FALSE, &perspective[0][0]);
  glUniformMatrix4fv(shaders->get_uniform_location("view"),
                     1, GL_FALSE, &view[0][0]);

  glBindVertexArray(VAO);

  for (unsigned int i = 0; i < NUM_CUBES; i++) {
    mat4 model = glm::translate(mat4(1.0f), CUBE_POSITIONS[i]);
    model = glm::rotate(model, 20.0f * i + static_cast<float>(time), vec3(1.0f, 0.3f, 0.5f));

    glUniformMatrix4fv(shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);

    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, buffer_offset(0));
  }

  //-----------------------------------------------------------------------------------------------
  // Draw Lights
  //-----------------------------------------------------------------------------------------------

  light_shaders->use_shader_program();

  glUniformMatrix4fv(light_shaders->get_uniform_location("perspective"),
                     1, GL_FALSE, &perspective[0][0]);
  glUniformMatrix4fv(light_shaders->get_uniform_location("view"),
                     1, GL_FALSE, &view[0][0]);
  glUniform3f(light_shaders->get_uniform_location("light_color"), 1.0f, 1.0f, 1.0f);

  glBindVertexArray(light_VAO);

  for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
    mat4 model = glm::translate(mat4(1.0f), POINT_LIGHT_POSITIONS[i]);
    model = glm::scale(model, vec3(0.2f));

    glUniformMatrix4fv(light_shaders->get_uniform_location("model"), 1, GL_FALSE, &model[0][0]);

    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, buffer_offset(0));
  }
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
  glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICIES), INDICIES, GL_STATIC_DRAW);

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
