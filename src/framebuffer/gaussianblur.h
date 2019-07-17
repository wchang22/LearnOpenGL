#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "framebuffer/framebuffer.h"

class GaussianBlur
{
public:
  GaussianBlur(int width, int height,
               const char* blur_vertex_path, const char* blur_frag_path,
               const char* fb_vertex_path, const char* fb_frag_path);

  void bind_framebuffer() const;
  void unbind_framebuffer() const;
  void blur_scene() const;

private:
  void blur() const;

  FrameBuffer hdr_buffer;
  FrameBuffer blur_buffer;
};

#endif // GAUSSIANBLUR_H
