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
  unsigned int UBO, SSBO;

  static void* buffer_offset(int offset);
  void init_buffers();
  void init_textures();
  void init_shaders();
  void draw_cubes() const;
  void draw_floor() const;
  void draw_model(const Shader& shader) const;
  void draw_skybox() const;
  void draw_planet() const;
  void draw_rock() const;

  unsigned int amount = 5000;
  std::unique_ptr<Shader> shaders;
  std::unique_ptr<Shader> skybox_shaders;
  std::unique_ptr<Shader> model_shaders;
  std::unique_ptr<Shader> asteroid_shaders;
  std::unique_ptr<Shader> planet_shaders;
  std::shared_ptr<Camera> camera;
  std::vector<mat4> model_matrices;
  Textures textures;
  Model model_nanosuit;
  Model model_planet;
  Model model_rock;
};

#endif // DISPLAY_H
