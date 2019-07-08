#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "shader/shader.h"

#include <glad/glad.h>
#include <memory>
#include <tuple>
#include <vector>

class FrameBuffer
{
  friend class GaussianBlur;

public:
  FrameBuffer(int width, int height,
              const char* vertex_path, const char* frag_path,
              unsigned int num_buffers = 1,
              int buffer_num_bits = 8,
              GLenum buffer_type = GL_UNSIGNED_BYTE,
              bool renderbuffer = true,
              bool stencil = false);
  virtual ~FrameBuffer();

  virtual void bind_framebuffer() const;
  virtual void unbind_framebuffer() const;
  virtual void draw_scene() const;

protected:
  static std::tuple<unsigned, unsigned, unsigned> get_buffer_types(int buffer_num_bits,
                                                                   GLenum buffer_type,
                                                                   bool stencil);

  unsigned int VAO, VBO, RBO, FBO;
  std::vector<unsigned int> color_textures;
  int width, height;
  Shader shader;
};

#endif // FRAMEBUFFER_H
