#include "gaussianblur.h"

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

void GaussianBlur::unbind_framebuffer() const
{
  hdr_buffer.unbind_framebuffer();
}

void GaussianBlur::blur() const
{
  constexpr int amount = 5;

  blur_buffer.shader.use_shader_program();

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(blur_buffer.shader.get_uniform_location("image"), 0);
  glBindVertexArray(blur_buffer.VAO);
  glBindFramebuffer(GL_FRAMEBUFFER, blur_buffer.FBO);

  glUniform1i(blur_buffer.shader.get_uniform_location("horizontal"), 0);
  glBindTexture(GL_TEXTURE_2D, hdr_buffer.color_textures[1]);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindTexture(GL_TEXTURE_2D, blur_buffer.color_textures.front());

  for (unsigned int i = 1; i < amount; i++) {
    glUniform1i(blur_buffer.shader.get_uniform_location("horizontal"), (i & 1) == 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GaussianBlur::draw_scene() const
{
  hdr_buffer.shader.use_shader_program();
  glDisable(GL_DEPTH_TEST);

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(hdr_buffer.shader.get_uniform_location("hdr"), 0);
  glBindTexture(GL_TEXTURE_2D, hdr_buffer.color_textures.front());

  glActiveTexture(GL_TEXTURE1);
  glUniform1i(hdr_buffer.shader.get_uniform_location("bloom"), 1);
  glBindTexture(GL_TEXTURE_2D, blur_buffer.color_textures.front());

  glBindVertexArray(hdr_buffer.VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
