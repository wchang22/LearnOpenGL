#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "shader/shader.h"
#include "framebuffer/framebuffer.h"

class GaussianBlur
{
public:
  GaussianBlur(int width, int height);

  void blur() const;

private:
  Shader shader;
  FrameBuffer buffers[2];
};

#endif // GAUSSIANBLUR_H
