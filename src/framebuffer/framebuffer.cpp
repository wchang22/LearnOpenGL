#include "framebuffer.h"
#include "util/exception.h"
#include "util/data.h"

FrameBuffer::FrameBuffer(int width, int height,
                         const char* vertex_path,
                         const char* frag_path,
                         const std::vector<GLenum>& buffer_formats,
                         bool renderbuffer,
                         bool stencil)
  : width(width),
    height(height),
    shader(std::make_shared<Shader>(vertex_path, frag_path))
{
  unsigned int rb_storage_type = stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT;
  unsigned int rb_attachment_type = stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

  glGenFramebuffers(1, &FBO);

  color_textures.resize(buffer_formats.size());
  glGenTextures(static_cast<int>(buffer_formats.size()), color_textures.data());

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  for (unsigned int i = 0; i < buffer_formats.size(); i++) {
    const auto [pixel_format, pixel_type] = get_pixel_format_type(buffer_formats[i]);

    glBindTexture(GL_TEXTURE_2D, color_textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(buffer_formats[i]),
                 width, height, 0, pixel_format, pixel_type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, color_textures[i], 0);
    textures.add_texture("texture_screen", color_textures[i]);
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FrameBufferException("Framebuffer not complete");
  }

  if (renderbuffer) {
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, rb_storage_type, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, rb_attachment_type, GL_RENDERBUFFER, RBO);
  }

  rect.start_setup();
  rect.add_vertices(QUAD_VERTICES, 6, sizeof (QUAD_VERTICES));
  rect.add_vertex_attribs({ 2, 2 });
  rect.finalize_setup();

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
  glDeleteFramebuffers(1, &FBO);
  glDeleteRenderbuffers(1, &RBO);
  glDeleteTextures(static_cast<int>(color_textures.size()), color_textures.data());
}

std::tuple<GLenum, GLenum> FrameBuffer::get_pixel_format_type(GLenum buffer_format)
{
  switch (buffer_format) {
    case GL_RGBA:
    case GL_RGBA16:
      return std::make_tuple(GL_RGBA, GL_UNSIGNED_BYTE);
    case GL_RGBA16F:
    case GL_RGBA32F:
      return std::make_tuple(GL_RGBA, GL_FLOAT);
    case GL_RGB:
    case GL_RGB16:
      return std::make_tuple(GL_RGB, GL_UNSIGNED_BYTE);
    case GL_RGB16F:
    case GL_RGB32F:
      return std::make_tuple(GL_RGB, GL_FLOAT);
    default:
      throw FrameBufferException("Unknown framebuffer type: " + std::to_string(buffer_format));
  }
}

void FrameBuffer::bind_framebuffer() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  if (color_textures.size() == 1) {
    return;
  }

  std::vector<unsigned int> attachments;

  for (unsigned int i = 0; i < color_textures.size(); i++) {
    attachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
  }

  glDrawBuffers(static_cast<int>(color_textures.size()), attachments.data());
}

void FrameBuffer::unbind_framebuffer() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void FrameBuffer::draw_scene() const
{
  glDisable(GL_DEPTH_TEST);
  rect.draw(*shader, textures);
}

void FrameBuffer::blit_depth() const
{
  glEnable(GL_DEPTH_TEST);
  int current_FBO;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_FBO);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<unsigned int>(current_FBO));
  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, static_cast<unsigned int>(current_FBO));
}

std::shared_ptr<Shader> FrameBuffer::get_shader() const
{
  return shader;
}
