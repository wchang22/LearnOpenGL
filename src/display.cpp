#include "display.h"
#include "exception.h"
#include "data.h"
#include "window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <algorithm>

const vec3 dir_light_dir = vec3(2.0f, -4.0f, 1.0f);
const vec3 point_light_pos = vec3(0.0f, 3.0f, 2.0f);

Display::Display(std::shared_ptr<Camera> camera)
  : camera(camera),
    model_nanosuit("../../assets/nanosuit_reflection/nanosuit.obj"),
    point_shadow(1024, 1024, Window::width(), Window::height(), point_light_pos)
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

  point_shadow.bind_depth_map();

  draw_model(*point_depth_shaders);
  draw_cubes(*point_depth_shaders);
  draw_box(*point_depth_shaders);

  point_shadow.bind_shadow_map("shadow_map", { shaders, model_shaders });
  set_lights();

  draw_cubes(*shaders);
  draw_box(*shaders);
  draw_model(*model_shaders);
  draw_lights(*light_shaders);
  draw_skybox();
}

void Display::generate_cube_vertices(float in[192], float out[504])
{
  for (unsigned int quad = 0; quad < 6; quad++) {
    unsigned int in_quad_offset = quad * 4 * 8;
    unsigned int out_quad_offset = quad * 6 * 14;

    vec3 pos[4];
    vec3 nm[4];
    vec2 uv[4];
    vec3 edge[2];
    vec2 delta_uv[2];
    vec3 tangent[2];
    vec3 bitangent[2];

    for (unsigned int vertex = 0; vertex < 4; vertex++) {
      unsigned int in_pos_offset = in_quad_offset + vertex * 8;
      pos[vertex] = vec3(in[in_pos_offset], in[in_pos_offset + 1], in[in_pos_offset + 2]);

      unsigned int in_nm_offset = in_pos_offset + 3;
      nm[vertex] = vec3(in[in_nm_offset], in[in_nm_offset + 1], in[in_nm_offset + 2]);

      unsigned int in_uv_offset = in_nm_offset + 3;
      uv[vertex] = vec2(in[in_uv_offset], in[in_uv_offset + 1]);
    }

    edge[0] = pos[1] - pos[0];
    edge[1] = pos[2] - pos[0];
    delta_uv[0] = uv[1] - uv[0];
    delta_uv[1] = uv[2] - uv[0];

    float f = 1.0f / (delta_uv[0].x * delta_uv[1].y - delta_uv[1].x * delta_uv[0].y);
    mat2 uv_adj_mat(delta_uv[1].y, -delta_uv[1].x, -delta_uv[0].y, delta_uv[0].x);
    mat3x2 edge_mat(edge[0].x, edge[1].x, edge[0].y, edge[1].y, edge[0].z, edge[1].z);
    mat3x2 tb_mat = f * uv_adj_mat * edge_mat;

    tangent[0] = glm::normalize(vec3(tb_mat[0][0], tb_mat[1][0], tb_mat[2][0]));
    bitangent[0] = glm::normalize(vec3(tb_mat[0][1], tb_mat[1][1], tb_mat[2][1]));

    edge[0] = pos[2] - pos[0];
    edge[1] = pos[3] - pos[0];
    delta_uv[0] = uv[2] - uv[0];
    delta_uv[1] = uv[3] - uv[0];

    f = 1.0f / (delta_uv[0].x * delta_uv[1].y - delta_uv[1].x * delta_uv[0].y);
    uv_adj_mat = mat2(delta_uv[1].y, -delta_uv[1].x, -delta_uv[0].y, delta_uv[0].x);
    edge_mat = mat3x2(edge[0].x, edge[1].x, edge[0].y, edge[1].y, edge[0].z, edge[1].z);
    tb_mat = f * uv_adj_mat * edge_mat;

    tangent[1] = glm::normalize(vec3(tb_mat[0][0], tb_mat[1][0], tb_mat[2][0]));
    bitangent[1] = glm::normalize(vec3(tb_mat[0][1], tb_mat[1][1], tb_mat[2][1]));

    float quad_vertices[] = {
      pos[0].x, pos[0].y, pos[0].z, nm[0].x, nm[0].y, nm[0].z, uv[0].x, uv[0].y,
      tangent[0].x, tangent[0].y, tangent[0].z, bitangent[0].x, bitangent[0].y, bitangent[0].z,

      pos[1].x, pos[1].y, pos[1].z, nm[1].x, nm[1].y, nm[1].z, uv[1].x, uv[1].y,
      tangent[0].x, tangent[0].y, tangent[0].z, bitangent[0].x, bitangent[0].y, bitangent[0].z,

      pos[2].x, pos[2].y, pos[2].z, nm[2].x, nm[2].y, nm[2].z, uv[2].x, uv[2].y,
      tangent[0].x, tangent[0].y, tangent[0].z, bitangent[0].x, bitangent[0].y, bitangent[0].z,


      pos[0].x, pos[0].y, pos[0].z, nm[0].x, nm[0].y, nm[0].z, uv[0].x, uv[0].y,
      tangent[1].x, tangent[1].y, tangent[1].z, bitangent[1].x, bitangent[1].y, bitangent[1].z,

      pos[2].x, pos[2].y, pos[2].z, nm[2].x, nm[2].y, nm[2].z, uv[2].x, uv[2].y,
      tangent[1].x, tangent[1].y, tangent[1].z, bitangent[1].x, bitangent[1].y, bitangent[1].z,

      pos[3].x, pos[3].y, pos[3].z, nm[3].x, nm[3].y, nm[3].z, uv[3].x, uv[3].y,
      tangent[1].x, tangent[1].y, tangent[1].z, bitangent[1].x, bitangent[1].y, bitangent[1].z
    };

    std::copy(std::begin(quad_vertices), std::end(quad_vertices), out + out_quad_offset);
  }
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


  float processed_vertices[504];
  generate_cube_vertices(cubeVertices, processed_vertices);

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);

  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(processed_vertices), processed_vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof (float), buffer_offset(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof (float), buffer_offset(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof (float), buffer_offset(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof (float), buffer_offset(11 * sizeof(float)));
  glEnableVertexAttribArray(4);


  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);

  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), buffer_offset(0));
  glEnableVertexAttribArray(0);


  glGenBuffers(1, &lightsUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
  glBufferData(GL_UNIFORM_BUFFER, 10 * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);


  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::init_textures() {
  textures.load_texture_from_image("../../assets/bricks2.jpg", "texture_diffuse");
  textures.load_texture_from_image("../../assets/bricks2_normal.jpg", "texture_normal");
  textures.load_texture_from_image("../../assets/bricks2_disp.jpg", "texture_height");
  toybox_textures.load_texture_from_image("../../assets/wood.png", "texture_diffuse");
  toybox_textures.load_texture_from_image("../../assets/toy_box_normal.png", "texture_normal");
  toybox_textures.load_texture_from_image("../../assets/toy_box_disp.png", "texture_height");
  skybox_textures.load_cubemap({
    "../../assets/space/right.jpg",
    "../../assets/space/left.jpg",
    "../../assets/space/top.jpg",
    "../../assets/space/bottom.jpg",
    "../../assets/space/front.jpg",
    "../../assets/space/back.jpg",
  });
}

void Display::init_shaders() {
  shaders = std::make_unique<Shader>("../../shaders/cube_vertex.glsl",
                                     "../../shaders/cube_fragment.glsl");
  light_shaders = std::make_unique<Shader>("../../shaders/light_vertex.glsl",
                                           "../../shaders/light_fragment.glsl");
  model_shaders = std::make_unique<Shader>("../../shaders/model_vertex.glsl",
                                           "../../shaders/model_fragment.glsl");
  skybox_shaders = std::make_unique<Shader>("../../shaders/skybox_vertex.glsl",
                                            "../../shaders/skybox_fragment.glsl");
  point_depth_shaders = std::make_unique<Shader>("../../shaders/point_depth_vertex.glsl",
                                                 "../../shaders/point_depth_fragment.glsl",
                                                 "../../shaders/point_depth_geometry.glsl");
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
    vec3(1.0f, 0.045f, 0.016f),
  };

  glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);

  float shininess = 32.0f;

  glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof (vec4), sizeof (vec3), &dir_light.direction[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (vec4), sizeof (vec3), &dir_light.ambient[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof (vec4), sizeof (vec3), &dir_light.diffuse[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof (vec4), sizeof (vec3), &dir_light.specular[0]);

  glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof (vec4), sizeof (vec3), &point_light_pos[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 5 * sizeof (vec4), sizeof (vec3), &point_light.ambient[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 6 * sizeof (vec4), sizeof (vec3), &point_light.diffuse[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 7 * sizeof (vec4), sizeof (vec3), &point_light.specular[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 8 * sizeof (vec4), sizeof (vec3), &point_light.attenuation[0]);

  glBufferSubData(GL_UNIFORM_BUFFER, 9 * sizeof (vec4), sizeof (vec3), &camera->get_position()[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, 9 * sizeof (vec4) + sizeof (vec3), sizeof (float), &shininess);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::draw_cubes(const Shader& shader) const
{
  shader.use_shader_program();

  glBindVertexArray(cubeVAO);
  toybox_textures.use_textures(shader);
  mat4 model(1.0f);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  model = glm::translate(mat4(1.0f), vec3(2.0f, 4.0f, 2.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  model = glm::translate(mat4(1.0f), vec3(-1.0f, 0.0f, -1.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  model = glm::translate(mat4(1.0f), vec3(2.0f, 0.0f, 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::draw_lights(const Shader& shader) const
{
  shader.use_shader_program();

  glBindVertexArray(cubeVAO);
  mat4 model(1.0f);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  model = glm::translate(model, point_light_pos);
  model = glm::scale(model, vec3(0.2f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::draw_box(const Shader& shader) const
{
  shader.use_shader_program();
  glUniform1i(shader.get_uniform_location("reverse_normal"), 1);

  glBindVertexArray(cubeVAO);
  textures.use_textures(shader);
  mat4 model(1.0f);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  model = glm::scale(model, vec3(10.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glUniform1i(shader.get_uniform_location("reverse_normal"), 0);
}

void Display::draw_model(const Shader& shader) const
{
  shader.use_shader_program();

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  glUniform1i(shader.get_uniform_location("gamma"), 1);
  mat4 model = glm::scale(mat4(1.0f), vec3(0.2f, 0.2, 0.2f));
  model = glm::rotate(model, -static_cast<float>(glfwGetTime()), vec3(0, 1, 0));
  model = glm::translate(model, vec3(0.0f, -2.5f, 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof (mat4), sizeof (mat4), &model[0][0]);
  model_nanosuit.draw(shader);
  glUniform1i(shader.get_uniform_location("gamma"), 0);

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
