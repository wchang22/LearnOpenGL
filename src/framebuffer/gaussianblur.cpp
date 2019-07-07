#include "gaussianblur.h"

GaussianBlur::GaussianBlur(int width, int height)
  : shader("../../shaders/processing/blur.vert", "../../shaders/processing/blur.frag"),
    buffers { FrameBuffer(width, height, 1, 16, GL_FLOAT, false, false),
              FrameBuffer(width, height, 1, 16, GL_FLOAT, false, false)}
{
}

void GaussianBlur::blur() const
{

}
