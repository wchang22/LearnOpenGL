#include "point_shadow.h"
#include "exception.h"

#include <glad/glad.h>

PointShadow::PointShadow(int width, int height, int window_width, int window_height, vec3 position)
  : Shadow (width, height, window_width, window_height)
{
  for (unsigned int i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, Shadow::FBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Shadow::depth_map, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, Shadow::FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Shadow::depth_map, 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw Exception::ShadowException("Failed to generate point shadow framebuffer");
  }

  mat4 perspective = glm::perspective(glm::radians(90.0f), static_cast<float>(width) / height,
                                      1.0f, 25.0f);
  std::vector<mat4> views(6);
  const std::pair<vec3, vec3> lookats[6] {
    { vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, -1.0f, 0.0f) },
    { vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) },
    { vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 0.0f, 1.0f) },
    { vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) },
    { vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, -1.0f, 0.0f) },
    { vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f) },
  };

  for (auto& lookat : lookats) {
    views.emplace_back(perspective * glm::lookAt(position, position + lookat.first, lookat.second));
  }

  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 6 * sizeof (mat4), views.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 9, UBO);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

PointShadow::~PointShadow()
{
  glDeleteBuffers(1, &UBO);
}
