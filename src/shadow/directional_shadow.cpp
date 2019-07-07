#include "directional_shadow.h"
#include "util/exception.h"

#include <glad/glad.h>

DirectionalShadow::DirectionalShadow(int width, int height, int window_width, int window_height,
                                     vec3 direction)
    : Shadow (width, height, window_width, window_height)
{
  glBindTexture(GL_TEXTURE_2D, Shadow::depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

  glBindFramebuffer(GL_FRAMEBUFFER, Shadow::FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Shadow::depth_map, 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw ShadowException("Failed to generate directional shadow framebuffer");
  }

  mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
  mat4 light_view = glm::lookAt(-direction, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
  mat4 light_space = light_projection * light_view;

  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4), &light_space[0][0], GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 8, UBO);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

DirectionalShadow::~DirectionalShadow()
{
  glDeleteBuffers(1, &UBO);
}
