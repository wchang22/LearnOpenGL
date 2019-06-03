#include "shadow.h"

Shadow::Shadow(int width, int height, int window_width, int window_height)
    : width(width), height(height),
      window_width(window_width), window_height(window_height),
      FBO(0), depth_map(0)
{
  glGenFramebuffers(1, &FBO);

  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw Exception::ShadowException("Failed to generate shadow framebuffer");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Shadow::~Shadow()
{
  glDeleteTextures(1, &depth_map);
  glDeleteFramebuffers(1, &FBO);
}

void Shadow::bind_depth_map() const
{
  glViewport(0, 0, width, height);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void Shadow::bind_shadow_map(const char* uniform_name,
                             const std::vector<std::shared_ptr<Shader>>& shaders) const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_width, window_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto shader : shaders) {
    shader->use_shader_program();
    glActiveTexture(GL_TEXTURE31);
    glUniform1i(shader->get_uniform_location(uniform_name), 31);
    glBindTexture(GL_TEXTURE_2D, depth_map);
  }
}
