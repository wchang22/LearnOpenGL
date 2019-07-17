#include "display.h"
#include "util/data.h"
#include "display/window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr vec3 POINT_LIGHT_POS = vec3(0.0f, 3.0f, 2.0f);

Display::Display(std::shared_ptr<Camera> camera)
  : camera(camera),
    model_nanosuit("../../assets/nanosuit_reflection/nanosuit.obj"),
    point_shadow(1024, 1024, Window::width(), Window::height(), POINT_LIGHT_POS),
    blur(Window::width(), Window::height(),
         "../../shaders/processing/blur.vert", "../../shaders/processing/blur.frag",
         "../../shaders/processing/fb.vert", "../../shaders/processing/fb.frag"),
    gbuffer(Window::width(), Window::height(),
            "../../shaders/processing/deferred.vert", "../../shaders/processing/deferred.frag",
            { GL_RGB16F, GL_RGBA16F, GL_RGBA, GL_RGB16F, GL_RGB16F, GL_RGB16F }),
    lights(camera)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_shaders();
  init_buffers();
  init_textures();
}

void Display::draw() const {
  Object::set_world_space_transform(camera->perspective(), camera->lookat());

//  point_shadow.bind_depth_map();

//  draw_model(*point_depth_shaders);
//  draw_cubes(*point_depth_shaders);

//  point_shadow.bind_shadow_map("shadow_map", { gbuffer.get_shader()});

  lights.update();

  gbuffer.bind_framebuffer();

  draw_cubes(*gbuffer_shaders);
  draw_box(*gbuffer_shaders);
  draw_model(*gbuffer_shaders);

  gbuffer.unbind_framebuffer();

  blur.bind_framebuffer();

  gbuffer.draw_scene();
  gbuffer.blit_depth();

  draw_lights(*light_shaders);
  draw_skybox(*skybox_shaders);

  blur.unbind_framebuffer();
  blur.blur_scene();
}

void Display::init_buffers() {
  float processed_vertices[504];
  generate_cube_vertices(CUBE_VERTICES, processed_vertices);

  cube.start_setup();
  cube.add_vertices(processed_vertices, 36, sizeof (processed_vertices));
  cube.add_vertex_attribs({ 3, 3, 2, 3, 3 });
  cube.finalize_setup();

  skybox.start_setup();
  skybox.add_vertices(SKYBOX_VERTICES, 36, sizeof (SKYBOX_VERTICES));
  skybox.add_vertex_attribs({ 3 });
  skybox.finalize_setup();

  lights.add_point_light({
     POINT_LIGHT_POS,
     vec3(0.05f),
     vec3(5.0f),
     vec3(3.0f),
     vec3(1.0f, 0.045f, 0.016f),
   });

  lights.add_point_light({
     vec3(-2.0f, 3.0f, 2.0f),
     vec3(0.05f),
     vec3(0.0f, 0.0f, 5.0f),
     vec3(1.0f, 1.0f, 3.0f),
     vec3(1.0f, 0.045f, 0.016f),
   });
}

void Display::init_textures() {
  cube_textures.load_texture_from_image("../../assets/bricks/bricks2.jpg", "texture_diffuse");
  cube_textures.load_texture_from_image("../../assets/bricks/bricks2_normal.jpg", "texture_normal");
  cube_textures.load_texture_from_image("../../assets/bricks/bricks2_disp.jpg", "texture_height");
  toybox_textures.load_texture_from_image("../../assets/box/wood.png", "texture_diffuse");
  toybox_textures.load_texture_from_image("../../assets/box/toy_box_normal.png", "texture_normal");
  toybox_textures.load_texture_from_image("../../assets/box/toy_box_disp.png", "texture_height");
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
  cube_shaders = std::make_shared<Shader>("../../shaders/object/cube.vert",
                                     "../../shaders/object/cube.frag");
  light_shaders = std::make_shared<Shader>("../../shaders/object/light.vert",
                                           "../../shaders/object/light.frag");
  model_shaders = std::make_shared<Shader>("../../shaders/object/model.vert",
                                           "../../shaders/object/model.frag");
  skybox_shaders = std::make_shared<Shader>("../../shaders/object/skybox.vert",
                                            "../../shaders/object/skybox.frag");
  point_depth_shaders = std::make_shared<Shader>("../../shaders/shadow/point_depth.vert",
                                                 "../../shaders/shadow/point_depth.frag",
                                                 "../../shaders/shadow/point_depth.geom");
  gbuffer_shaders = std::make_shared<Shader>("../../shaders/processing/gbuffer.vert",
                                             "../../shaders/processing/gbuffer.frag");
}

void Display::draw_cubes(const Shader& shader) const
{
  static const std::vector<Object::Transform> transforms {
    { {}, {}, vec3(0.0f, -2.0f, 0.0f) },
    { {}, {}, vec3(2.0f, 4.0f, 2.0f) },
    { {}, {}, vec3(-1.0f, 0.0f, -1.0f) },
    { {}, {}, vec3(2.0f, 0.0f, 0.0f) }
  };

  Object::set_model_transforms(transforms);
  cube.draw_instanced(shader, 3, toybox_textures);
}

void Display::draw_lights(const Shader& shader) const
{
  lights.draw(shader);
}

void Display::draw_box(const Shader& shader) const
{
  Object::set_model_transforms({
   { vec3(15.0f), {}, {} }
  });
  cube.draw(shader, cube_textures, { "reverse_normal" });
}

void Display::draw_model(const Shader& shader) const
{
  Object::set_model_transforms({
    {
      vec3(0.2f),
      std::make_pair(-static_cast<float>(glfwGetTime()), vec3(0.0f, 1.0f, 0.0f)),
      vec3(0.0f, -0.5f, 0.0f)
    },
  });
  model_nanosuit.draw(shader, { "gamma" });
}

void Display::draw_skybox(const Shader& shader) const
{
  glDepthFunc(GL_LEQUAL);

  Object::set_world_space_transform(camera->perspective(), mat4(mat3(camera->lookat())));
  skybox.draw(shader, skybox_textures);

  glDepthFunc(GL_LESS);
}
