#ifndef SHADOW_H
#define SHADOW_H

#include "shader.h"

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef glm::mat4 mat4;
typedef glm::vec3 vec3;

class Shadow
{
public:
  Shadow(int width, int height, int window_width, int window_height);
  virtual ~Shadow();

  virtual void bind_depth_map() const;
  virtual void bind_shadow_map(const char* uniform_name,
                               const std::vector<std::shared_ptr<Shader>>& shaders) const;

protected:
  int width, height, window_width, window_height;
  unsigned int FBO, depth_map;
};

#endif // SHADOW_H
