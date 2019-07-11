#include "display.h"
#include "util/data.h"
#include "display/window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr vec3 point_light_pos = vec3(0.0f, 3.0f, 2.0f);

Display::Display(std::shared_ptr<Camera> camera)
  : camera(camera),
    model_nanosuit("../../assets/nanosuit_reflection/nanosuit.obj"),
    point_shadow(1024, 1024, Window::width(), Window::height(), point_light_pos),
    blur(Window::width(), Window::height(),
         "../../shaders/processing/blur.vert", "../../shaders/processing/blur.frag",
         "../../shaders/processing/fb.vert", "../../shaders/processing/fb.frag"),
    lights(camera)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_shaders();
  init_buffers();
  init_textures();
}

void Display::draw() const {
  Object::set_world_space_transform(camera->perspective(), camera->lookat());

  point_shadow.bind_depth_map();

  draw_model(*point_depth_shaders);
  draw_cubes(*point_depth_shaders);
  draw_box(*point_depth_shaders);

  point_shadow.bind_shadow_map("shadow_map", { cube_shaders, model_shaders });

  lights.update();

  blur.bind_framebuffer();

  draw_cubes(*cube_shaders);
  draw_box(*cube_shaders);
  draw_model(*model_shaders);
  draw_lights(*light_shaders);
  draw_skybox();

  blur.blur_scene();
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

  lights.add_point_light({
     point_light_pos,
     vec3(0.05f),
     vec3(5.0f),
     vec3(3.0f),
     vec3(1.0f, 0.045f, 0.016f),
   });
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
  Object::set_model_transform(vec3(0.2f), {}, lights.get_point_light_pos(0));
  lights.draw(shader);
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
