#include "multisampleframebuffer.h"
#include "util/exception.h"
#include "util/data.h"

MultiSampleFrameBuffer::MultiSampleFrameBuffer(int width, int height,
                                               const char* vertex_path, const char* frag_path,
                                               unsigned int num_buffers,
                                               int buffer_num_bits,
                                               GLenum buffer_type,
                                               bool renderbuffer,
                                               bool stencil)
  : FrameBuffer (width, height, vertex_path, frag_path,
                 num_buffers, buffer_num_bits, buffer_type, false, stencil)
{
  auto [color_buffer_format, rb_storage_type, rb_attachment_type] =
      get_buffer_types(buffer_num_bits, buffer_type, stencil);

  glGenFramebuffers(1, &multiFBO);

  multi_color_textures.resize(num_buffers);
  glGenTextures(static_cast<int>(num_buffers), multi_color_textures.data());

  glBindFramebuffer(GL_FRAMEBUFFER, multiFBO);

  for (unsigned int i = 0; i < num_buffers; i++) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multi_color_textures[i]);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, num_aa_samples,
                            color_buffer_format, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D_MULTISAMPLE, multi_color_textures[i], 0);
  }

  if (renderbuffer) {
    glGenRenderbuffers(1, &multiRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, multiRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, num_aa_samples, rb_storage_type, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, rb_attachment_type, GL_RENDERBUFFER, multiRBO);
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FrameBufferException("Multisample Framebuffer not complete");
  }

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MultiSampleFrameBuffer::~MultiSampleFrameBuffer()
{
  glDeleteFramebuffers(1, &multiFBO);
  glDeleteTextures(static_cast<int>(multi_color_textures.size()), multi_color_textures.data());
}

void MultiSampleFrameBuffer::bind_framebuffer() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, multiFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  if (multi_color_textures.size() == 1) {
    return;
  }

  std::vector<unsigned int> attachments;
  attachments.reserve(multi_color_textures.size());

  for (unsigned int i = 0; i < multi_color_textures.size(); i++) {
    attachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
  }

  glDrawBuffers(static_cast<int>(multi_color_textures.size()), attachments.data());
}

void MultiSampleFrameBuffer::unbind_framebuffer() const
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, multiFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

  for (unsigned int i = 0; i < color_textures.size(); i++) {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  }

  FrameBuffer::unbind_framebuffer();
}
