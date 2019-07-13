#ifndef DISPLAY_H
#define DISPLAY_H

#include <memory>

#include <glm/glm.hpp>

#include "display/camera.h"
#include "shader/shader.h"
#include "shader/textures.h"
#include "model/model.h"
#include "model/object.h"
#include "model/lights.h"
#include "shadow/point_shadow.h"
#include "framebuffer/gaussianblur.h"
#include "framebuffer/multisampleframebuffer.h"

typedef glm::vec3 vec3;
typedef glm::mat3 mat3;

class Display {
public:
  Display(std::shared_ptr<Camera> camera);

  void draw() const;

private:
  void init_buffers();
  void init_textures();
  void init_shaders();
  void draw_cubes(const Shader& shader) const;
  void draw_lights(const Shader& shader) const;
  void draw_box(const Shader& shader) const;
  void draw_model(const Shader& shader) const;
  void draw_skybox(const Shader& shader) const;

  std::shared_ptr<Shader> cube_shaders;
  std::shared_ptr<Shader> skybox_shaders;
  std::shared_ptr<Shader> model_shaders;
  std::shared_ptr<Shader> point_depth_shaders;
  std::shared_ptr<Shader> light_shaders;
  std::shared_ptr<Camera> camera;

  Textures cube_textures;
  Textures toybox_textures;
  Textures skybox_textures;

  Object skybox;
  Object cube;

  Model model_nanosuit;
  PointShadow point_shadow;
  GaussianBlur blur;

  Lights lights;
};

#endif // DISPLAY_H
