#include "display.h"
#include "util/exception.h"
#include "util/data.h"
#include "display/window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <algorithm>

constexpr vec3 dir_light_dir = vec3(2.0f, -4.0f, 1.0f);
constexpr vec3 point_light_pos = vec3(0.0f, 3.0f, 2.0f);

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 attenuation;
};

constexpr DirLight dir_light {
  dir_light_dir,
  vec3(0.05f),
  vec3(0.4f),
  vec3(0.5f),
};

constexpr PointLight point_light {
  vec3(0.05f),
  vec3(5.0f),
  vec3(3.0f),
  vec3(1.0f, 0.045f, 0.016f),
};

Display::Display(std::shared_ptr<Camera> camera)
  : camera(camera),
    model_nanosuit("../../assets/nanosuit_reflection/nanosuit.obj"),
    point_shadow(1024, 1024, Window::width(), Window::height(), point_light_pos),
    blur(Window::width(), Window::height(),
         "../../shaders/processing/blur.vert", "../../shaders/processing/blur.frag",
         "../../shaders/processing/fb.vert", "../../shaders/processing/fb.frag")
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_shaders();
  init_buffers();
  init_textures();
  init_lights();
}

Display::~Display() {
  glDeleteBuffers(1, &lightsUBO);
}

void Display::draw() const {
  Object::set_world_space_transform(camera->perspective(), camera->lookat());

  point_shadow.bind_depth_map();

  draw_model(*point_depth_shaders);
  draw_cubes(*point_depth_shaders);
  draw_box(*point_depth_shaders);

  point_shadow.bind_shadow_map("shadow_map", { cube_shaders, model_shaders });

  glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 9 * sizeof (vec4), sizeof (vec3), &camera->get_position()[0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  blur.bind_framebuffer();

  draw_cubes(*cube_shaders);
  draw_box(*cube_shaders);
  draw_model(*model_shaders);
  draw_lights(*light_shaders);
  draw_skybox();

  blur.blur_scene();
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
  float processed_vertices[504];
  generate_cube_vertices(cubeVertices, processed_vertices);

  cube.start_setup();
  cube.add_vertices(processed_vertices, 36, sizeof (processed_vertices));
  cube.add_vertex_attribs({ 3, 3, 2, 3, 3 });
  cube.finalize_setup();

  skybox.start_setup();
  skybox.add_vertices(skyboxVertices, 36, sizeof (skyboxVertices));
  skybox.add_vertex_attribs({ 3 });
  skybox.finalize_setup();

  glGenBuffers(1, &lightsUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
  glBufferData(GL_UNIFORM_BUFFER, 10 * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display::init_textures() {
  cube_textures.load_texture_from_image("../../assets/bricks2.jpg", "texture_diffuse");
  cube_textures.load_texture_from_image("../../assets/bricks2_normal.jpg", "texture_normal");
  cube_textures.load_texture_from_image("../../assets/bricks2_disp.jpg", "texture_height");
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
  cube_shaders = std::make_unique<Shader>("../../shaders/object/cube.vert",
                                     "../../shaders/object/cube.frag");
  light_shaders = std::make_unique<Shader>("../../shaders/object/light.vert",
                                           "../../shaders/object/light.frag");
  model_shaders = std::make_unique<Shader>("../../shaders/object/model.vert",
                                           "../../shaders/object/model.frag");
  skybox_shaders = std::make_unique<Shader>("../../shaders/object/skybox.vert",
                                            "../../shaders/object/skybox.frag");
  point_depth_shaders = std::make_unique<Shader>("../../shaders/shadow/point_depth.vert",
                                                 "../../shaders/shadow/point_depth.frag",
                                                 "../../shaders/shadow/point_depth.geom");
}

void Display::init_lights() const
{
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
  constexpr vec3 positions[] = {
    vec3(0.0f, -2.0f, 0.0f),
    vec3(2.0f, 4.0f, 2.0f),
    vec3(-1.0f, 0.0f, -1.0f),
    vec3(2.0f, 0.0f, 0.0f)
  };

  for (const auto& position : positions) {
    Object::set_model_transform({}, {}, position);
    cube.draw(shader, toybox_textures);
  }
}

void Display::draw_lights(const Shader& shader) const
{
  Object::set_model_transform(vec3(0.2f), {}, point_light_pos);
  cube.draw(shader);
}

void Display::draw_box(const Shader& shader) const
{
  Object::set_model_transform(vec3(10.0f), {}, {});
  cube.draw(shader, cube_textures, { "reverse_normal" });
}

void Display::draw_model(const Shader& shader) const
{
  Object::set_model_transform(vec3(0.2f),
                              std::make_pair(-static_cast<float>(glfwGetTime()),
                                             vec3(0.0f, 1.0f, 0.0f)),
                              vec3(0.0f, -0.5f, 0.0f));
  model_nanosuit.draw(shader, { "gamma" });
}

void Display::draw_skybox() const
{
  glDepthFunc(GL_LEQUAL);

  Object::set_world_space_transform(camera->perspective(), mat4(mat3(camera->lookat())));
  skybox.draw(*skybox_shaders, skybox_textures);

  glDepthFunc(GL_LESS);
}
