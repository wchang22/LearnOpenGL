#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "shader/shader.h"
#include "model/object.h"
#include "shader/textures.h"

#include <glad/glad.h>
#include <tuple>
#include <vector>
#include <memory>

class FrameBuffer
{
  friend class GaussianBlur;
  friend class SSAO;

public:
  FrameBuffer(int width, int height,
              const char* vertex_path, const char* frag_path,
              const std::vector<GLenum>& buffer_formats = { GL_RGBA },
              bool renderbuffer = true,
              bool stencil = false);
  virtual ~FrameBuffer();

  virtual void bind_framebuffer() const;
  virtual void unbind_framebuffer() const;
  virtual void draw_scene() const;
  virtual void use_textures(const Shader& shader,
                            std::initializer_list<unsigned int> textures) const;
  virtual void blit_depth() const;
  virtual void blit_color(unsigned int from, unsigned int to) const;
  virtual std::shared_ptr<Shader> get_shader() const;

protected:
  static std::tuple<GLenum, GLenum> get_pixel_format_type(GLenum buffer_format);

  unsigned int RBO, FBO;
  std::vector<unsigned int> color_textures;
  int width, height;
  std::shared_ptr<Shader> shader;
  Object rect;
  Textures textures;
};

#endif // FRAMEBUFFER_H
