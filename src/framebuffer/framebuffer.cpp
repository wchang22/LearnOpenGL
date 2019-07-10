#include "framebuffer.h"
#include "util/exception.h"
#include "util/data.h"

FrameBuffer::FrameBuffer(int width, int height,
                         const char* vertex_path,
                         const char* frag_path,
                         unsigned int num_buffers,
                         int buffer_num_bits,
                         GLenum buffer_type,
                         bool renderbuffer,
                         bool stencil)
  : width(width),
    height(height),
    shader(vertex_path, frag_path)
{
  auto [color_buffer_format, rb_storage_type, rb_attachment_type] =
      get_buffer_types(buffer_num_bits, buffer_type, stencil);

  glGenFramebuffers(1, &FBO);

  color_textures.resize(num_buffers);
  glGenTextures(static_cast<int>(num_buffers), color_textures.data());

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  for (unsigned int i = 0; i < num_buffers; i++) {
    glBindTexture(GL_TEXTURE_2D, color_textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(color_buffer_format),
                 width, height, 0, GL_RGBA, buffer_type, nullptr);
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
  rect.add_vertices(quadVertices, 6, sizeof (quadVertices));
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

std::tuple<unsigned, unsigned, unsigned> FrameBuffer::get_buffer_types(int buffer_num_bits,
                                                                       GLenum buffer_type,
                                                                       bool stencil)
{
  unsigned int color_buffer_format;
  unsigned int rb_storage_type = stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT;
  unsigned int rb_attachment_type = stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

  if (buffer_type == GL_UNSIGNED_BYTE) {
    switch (buffer_num_bits) {
      case 8:
        color_buffer_format = GL_RGBA;
        break;
      case 16:
        color_buffer_format = GL_RGBA16;
        break;
      default:
        throw FrameBufferException("Invalid framebuffer size (bits) for byte type: " +
                                   std::to_string(buffer_num_bits));
    }
  } else if (buffer_type == GL_FLOAT) {
    switch (buffer_num_bits) {
      case 16:
        color_buffer_format = GL_RGBA16F;
        break;
      case 32:
        color_buffer_format = GL_RGBA32F;
        break;
      default:
        throw FrameBufferException("Invalid framebuffer size (bits) for float type: " +
                                   std::to_string(buffer_num_bits));
    }
  } else {
    throw FrameBufferException("Invalid framebuffer type");
  }

  return std::make_tuple(color_buffer_format, rb_storage_type, rb_attachment_type);
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
  rect.draw(shader, textures);
}
