#ifndef DISPLAY_H
#define DISPLAY_H

#include <memory>

#include <glm/glm.hpp>

#include "camera.h"
#include "shader/shader.h"
#include "shader/textures.h"
#include "model/model.h"
#include "shadow/directional_shadow.h"
#include "shadow/point_shadow.h"
#include "framebuffer/gaussianblur.h"

typedef glm::vec3 vec3;
typedef glm::mat3 mat3;
typedef glm::mat2 mat2;
typedef glm::mat3x2 mat3x2;

class Display {
public:
  Display(std::shared_ptr<Camera> camera);
  ~Display();

  void draw() const;

private:
  unsigned int cubeVAO, cubeVBO, planeVAO, planeVBO;
  unsigned int skyboxVAO, skyboxVBO;
  unsigned int UBO, lightsUBO;

  static void* buffer_offset(int offset);
  void generate_cube_vertices(float in[192], float out[504]);
  void init_buffers();
  void init_textures();
  void init_shaders();
  void set_lights() const;
  void draw_cubes(const Shader& shader) const;
  void draw_lights(const Shader& shader) const;
  void draw_box(const Shader& shader) const;
  void draw_model(const Shader& shader) const;
  void draw_skybox() const;

  std::shared_ptr<Shader> shaders;
  std::shared_ptr<Shader> skybox_shaders;
  std::shared_ptr<Shader> model_shaders;
  std::shared_ptr<Shader> point_depth_shaders;
  std::shared_ptr<Shader> light_shaders;
  std::shared_ptr<Camera> camera;
  Textures textures;
  Textures toybox_textures;
  Textures skybox_textures;
  Model model_nanosuit;
  PointShadow point_shadow;
  GaussianBlur fb;
};

#endif // DISPLAY_H
