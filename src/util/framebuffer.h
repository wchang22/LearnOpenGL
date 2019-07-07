#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "shader/shader.h"

#include <glad/glad.h>
#include <memory>

class FrameBuffer
{
public:
  FrameBuffer(int width, int height, int buffer_num_bits = 8,
              GLenum buffer_type = GL_UNSIGNED_BYTE, bool stencil = false);
  ~FrameBuffer();

  void bind_framebuffer() const;
  void draw_scene() const;

private:
  unsigned int VAO, VBO, FBO, RBO, color_texture;
  Shader shader;
};

#endif // FRAMEBUFFER_H
