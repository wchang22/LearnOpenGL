#ifndef POINT_SHADOW_H
#define POINT_SHADOW_H

#include "shadow/shadow.h"

class PointShadow : public Shadow
{
public:
  PointShadow(int width, int height, int window_width, int window_height, vec3 position);
  ~PointShadow() override;

  void bind_shadow_map(const char* uniform_name,
                       std::initializer_list<std::shared_ptr<Shader>> shaders) const override;

private:
  unsigned int UBO;
};

#endif // POINT_SHADOW_H
