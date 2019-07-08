#include "gaussianblur.h"

static bool horizontal = true;

GaussianBlur::GaussianBlur(int width, int height,
                           const char* blur_vertex_path, const char* blur_frag_path,
                           const char* fb_vertex_path, const char* fb_frag_path)
  : hdr_buffer(width, height, fb_vertex_path, fb_frag_path, 2, 16, GL_FLOAT, true, false),
    blur_buffers { FrameBuffer(width, height, blur_vertex_path, blur_frag_path,
                               1, 16, GL_FLOAT, false, false),
                   FrameBuffer(width, height, blur_vertex_path, blur_frag_path,
                               1, 16, GL_FLOAT, false, false)}
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
  constexpr int amount = 10;

  blur_buffers[0].shader.use_shader_program();

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(blur_buffers[0].shader.get_uniform_location("image"), 0);
  glBindVertexArray(blur_buffers[0].VAO);

  for (unsigned int i = 0; i < amount; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, blur_buffers[horizontal].FBO);
    glUniform1i(blur_buffers[0].shader.get_uniform_location("horizontal"), horizontal);
    glBindTexture(GL_TEXTURE_2D, i == 0 ? hdr_buffer.color_textures[1] :
                                          blur_buffers[!horizontal].color_textures.front());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    horizontal = !horizontal;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GaussianBlur::draw_scene() const
{
  hdr_buffer.shader.use_shader_program();
  glDisable(GL_DEPTH_TEST);

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(hdr_buffer.shader.get_uniform_location("hdr"), 0);
  glBindTexture(GL_TEXTURE_2D, hdr_buffer.color_textures[0]);

  glActiveTexture(GL_TEXTURE1);
  glUniform1i(hdr_buffer.shader.get_uniform_location("bloom"), 1);
  glBindTexture(GL_TEXTURE_2D, blur_buffers[!horizontal].color_textures.front());

  glBindVertexArray(hdr_buffer.VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
