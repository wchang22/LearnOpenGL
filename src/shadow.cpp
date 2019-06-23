#include "shadow.h"

#include <glad/glad.h>

Shadow::Shadow(int width, int height, int window_width, int window_height)
  : width(width), height(height),
    window_width(window_width), window_height(window_height),
    FBO(0), depth_map(0)
{
  glGenFramebuffers(1, &FBO);
  glGenTextures(1, &depth_map);
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
