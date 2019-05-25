#ifndef DISPLAY_H
#define DISPLAY_H

#include <memory>

#include <glm/glm.hpp>

#include "camera.h"
#include "shader.h"
#include "textures.h"
#include "model.h"

typedef glm::vec3 vec3;
typedef glm::mat3 mat3;

class Display {
public:
  Display(std::shared_ptr<Camera> camera);
  ~Display();

  void draw() const;

private:
  unsigned int cubeVAO, cubeVBO, planeVAO, planeVBO;
  unsigned int skyboxVAO, skyboxVBO;
  unsigned int UBO;

  static void* buffer_offset(int offset);
  void init_buffers();
  void init_textures();
  void init_shaders();
  void draw_cubes() const;
  void draw_floor() const;
  void draw_model(const Shader& shader) const;
  void draw_skybox() const;

  std::unique_ptr<Shader> shaders;
  std::unique_ptr<Shader> skybox_shaders;
  std::unique_ptr<Shader> model_shaders;
  std::unique_ptr<Shader> model_geo_shaders;
  std::shared_ptr<Camera> camera;
  Textures textures;
  Model model_nanosuit;
};

#endif // DISPLAY_H
