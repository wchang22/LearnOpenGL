#ifndef MULTISAMPLEFRAMEBUFFER_H
#define MULTISAMPLEFRAMEBUFFER_H

#include "framebuffer/framebuffer.h"

#include <glad/glad.h>
#include <vector>

class MultiSampleFrameBuffer : public FrameBuffer
{
public:
  MultiSampleFrameBuffer(int width, int height,
                         const char* vertex_path, const char* frag_path,
                         const std::vector<GLenum>& buffer_formats = { GL_RGBA },
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
