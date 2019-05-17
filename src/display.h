#ifndef DISPLAY_H
#define DISPLAY_H

#include <memory>

#include <glm/glm.hpp>

#include "camera.h"
#include "shader.h"
#include "textures.h"
#include "model.h"

typedef glm::vec3 vec3;

class Display {
public:
  Display(std::shared_ptr<Camera> camera);
  ~Display();

  void draw() const;

private:
  unsigned int cubeVAO, cubeVBO, planeVAO, planeVBO, transparentVAO, transparentVBO;

  static void* buffer_offset(int offset);
  void init_buffers();
  void init_textures();
  void init_shaders();
  void draw_cubes() const;
  void draw_floor() const;
  void draw_transparent() const;

  std::unique_ptr<Shader> shaders;
  std::shared_ptr<Camera> camera;
  Textures metal_texture;
  Textures marble_texture;
  Textures transparent_texture;
};

#endif // DISPLAY_H
