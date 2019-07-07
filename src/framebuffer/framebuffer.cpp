#include "framebuffer.h"
#include "util/exception.h"
#include "util/data.h"

FrameBuffer::FrameBuffer(int width, int height,
                         unsigned int num_buffers,
                         int buffer_num_bits,
                         GLenum buffer_type,
                         bool renderbuffer,
                         bool stencil)
  : width(width),
    height(height),
    shader("../../shaders/processing/fb.vert", "../../shaders/processing/fb.frag")
{
  auto [color_buffer_format, rb_storage_type, rb_attachment_type] =
      get_buffer_types(buffer_num_bits, buffer_type, stencil);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
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

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof (quadVertices), quadVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float),
                        reinterpret_cast<void*>(2 * sizeof (float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
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
}

void FrameBuffer::unbind_framebuffer() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void FrameBuffer::draw_scene() const
{
  shader.use_shader_program();
  glDisable(GL_DEPTH_TEST);

  for (unsigned int i = 0; i < color_textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glUniform1i(shader.get_uniform_location("screen_texture" + std::to_string(i + 1)),
                static_cast<int>(i));
    glBindTexture(GL_TEXTURE_2D, color_textures[i]);
  }

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
