#ifndef SHADOW_H
#define SHADOW_H

#include "exception.h"
#include "shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

typedef glm::vec2 vec2;

class Shadow
{
public:
  Shadow(int width, int height, int window_width, int window_height);
  ~Shadow();

  void bind_depth_map() const;
  void bind_shadow_map(const char* uniform_name,
                       const std::vector<std::shared_ptr<Shader>>& shaders) const;

private:
  int width, height, window_width, window_height;
  unsigned int FBO, depth_map;
};

#endif // SHADOW_H
