#ifndef POINT_SHADOW_H
#define POINT_SHADOW_H

#include "shadow.h"

class PointShadow : public Shadow
{
public:
  PointShadow(int width, int height, int window_width, int window_height, vec3 position);
  ~PointShadow();

private:
  unsigned int UBO;
};

#endif // POINT_SHADOW_H
