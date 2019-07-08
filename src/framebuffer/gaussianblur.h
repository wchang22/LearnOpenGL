#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "framebuffer/multisampleframebuffer.h"

class GaussianBlur
{
public:
  GaussianBlur(int width, int height,
               const char* blur_vertex_path, const char* blur_frag_path,
               const char* fb_vertex_path, const char* fb_frag_path);

  void bind_framebuffer() const;
  void unbind_framebuffer() const;
  void blur() const;
  void draw_scene() const;

private:
  FrameBuffer hdr_buffer;
  FrameBuffer blur_buffers[2];
};

#endif // GAUSSIANBLUR_H
