#ifndef MULTISAMPLEFRAMEBUFFER_H
#define MULTISAMPLEFRAMEBUFFER_H

#include "shader/shader.h"
#include "framebuffer/framebuffer.h"

#include <glad/glad.h>
#include <memory>
#include <tuple>
#include <vector>

class MultiSampleFrameBuffer : public FrameBuffer
{
public:
  MultiSampleFrameBuffer(int width, int height,
                         const char* vertex_path, const char* frag_path,
                         unsigned int num_buffers = 1,
                         int buffer_num_bits = 8,
                         GLenum buffer_type = GL_UNSIGNED_BYTE,
                         bool renderbuffer = true,
                         bool stencil = false);
  ~MultiSampleFrameBuffer() override;

  void bind_framebuffer() const override;
  void unbind_framebuffer() const override;

private:
  unsigned int multiFBO, multiRBO;
  std::vector<unsigned int> multi_color_textures;
};

#endif // MULTISAMPLEFRAMEBUFFER_H
