#ifndef SSAO_H
#define SSAO_H

#include "framebuffer/framebuffer.h"
#include "shader/shader.h"

typedef glm::vec4 vec4;
typedef glm::vec2 vec2;

class SSAO
{
public:
  SSAO(int width, int height,
       const char* vertex_path, const char* frag_path);
  ~SSAO();

  void bind_framebuffer() const;
  void render() const;
  void use_ssao(const Shader& shader) const;
  std::shared_ptr<Shader> get_shader() const;

private:
  unsigned int noise_texture, UBO;
  FrameBuffer fb;
  vec2 dims;
};

#endif // SSAO_H
