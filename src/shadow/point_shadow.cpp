#include "point_shadow.h"
#include "util/exception.h"

#include <glad/glad.h>

PointShadow::PointShadow(int width, int height, int window_width, int window_height, vec3 position)
  : Shadow (width, height, window_width, window_height)
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow::depth_map);
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

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw ShadowException("Failed to generate point shadow framebuffer");
  }

  const float near_plane = 1.0f;
  const float far_plane = 25.0f;
  mat4 perspective = glm::perspective(glm::radians(90.0f), static_cast<float>(width) / height,
                                      near_plane, far_plane);
  std::vector<mat4> views;
  const std::pair<vec3, vec3> lookats[6] {
    { vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, -1.0f, 0.0f) },
    { vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) },
    { vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 0.0f, 1.0f) },
    { vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) },
    { vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, -1.0f, 0.0f) },
    { vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f) },
  };

  for (const auto& [center, up] : lookats) {
    views.emplace_back(perspective * glm::lookAt(position, position + center, up));
  }

  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 6 * sizeof (mat4) + sizeof(vec4) + sizeof (float),
               nullptr, GL_STATIC_DRAW);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, 6 * sizeof (mat4), views.data());
  glBufferSubData(GL_UNIFORM_BUFFER, 6 * sizeof (mat4), sizeof(vec3), &position);
  glBufferSubData(GL_UNIFORM_BUFFER, 6 * sizeof (mat4) + sizeof (vec3), sizeof(float), &far_plane);
  glBindBufferBase(GL_UNIFORM_BUFFER, 9, UBO);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

PointShadow::~PointShadow()
{
  glDeleteBuffers(1, &UBO);
}

void PointShadow::bind_shadow_map(const char* uniform_name,
                                  const std::vector<std::shared_ptr<Shader>>& shaders) const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, Shadow::window_width, Shadow::window_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto& shader : shaders) {
    shader->use_shader_program();
    glActiveTexture(GL_TEXTURE31);
    glUniform1i(shader->get_uniform_location(uniform_name), 31);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow::depth_map);
  }
}
