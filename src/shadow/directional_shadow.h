#ifndef DIRECTIONAL_SHADOW_H
#define DIRECTIONAL_SHADOW_H

#include "shadow/shadow.h"

class DirectionalShadow : public Shadow
{
public:
  DirectionalShadow(int width, int height, int window_width, int window_height, vec3 direction);
  ~DirectionalShadow() override;

private:
  unsigned int UBO;
};

#endif // DIRECTIONAL_SHADOW_H
