#include "gaussianblur.h"
#include <iostream>

GaussianBlur::GaussianBlur(int width, int height,
                           const char* blur_vertex_path, const char* blur_frag_path,
                           const char* fb_vertex_path, const char* fb_frag_path)
  : hdr_buffer(width, height, fb_vertex_path, fb_frag_path, 2, 16, GL_FLOAT, true, false),
    blur_buffer(width, height, blur_vertex_path, blur_frag_path, 1, 16, GL_FLOAT, false, false)
{
}

void GaussianBlur::bind_framebuffer() const
{
  hdr_buffer.bind_framebuffer();
}

void GaussianBlur::blur() const
{
  constexpr int amount = 5;

  glBindFramebuffer(GL_FRAMEBUFFER, blur_buffer.FBO);

  Textures textures;
  textures.add_texture("image", hdr_buffer.color_textures[1]);
  blur_buffer.rect.draw(blur_buffer.shader, textures);

  textures.clear();
  textures.add_texture("image", blur_buffer.color_textures.front());

  for (unsigned int i = 1; i < amount; i++) {
    if ((i & 1) == 1) {
      blur_buffer.rect.draw(blur_buffer.shader, textures, { "horizontal" });
    } else {
      blur_buffer.rect.draw(blur_buffer.shader, textures);
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GaussianBlur::blur_scene() const
{
  glDisable(GL_DEPTH_TEST);

  hdr_buffer.unbind_framebuffer();
  blur();

  Textures textures;
  textures.add_texture("hdr", hdr_buffer.color_textures.front());
  textures.add_texture("bloom", blur_buffer.color_textures.front());
  hdr_buffer.rect.draw(hdr_buffer.shader, textures);
}
