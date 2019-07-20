#include "ssao.h"

#include <random>

float lerp(float a, float b, float f)
{
  return a + f * (b - a);
}

std::vector<vec3> generate_ssao_kernel()
{
  std::uniform_real_distribution<float> random_floats(0.0, 1.0);
  std::default_random_engine generator;
  std::vector<vec3> ssao_kernel;

  for (unsigned int i = 0; i < 64; i++) {
    vec3 sample(random_floats(generator) * 2.0f - 1.0f,
                random_floats(generator) * 2.0f - 1.0f,
                random_floats(generator));
    sample = glm::normalize(sample);
    sample *= random_floats(generator);
    float scale = static_cast<float>(i) / 64.0f;
    scale = lerp(0.1f, 1.0f, scale * scale);
    sample *= scale;
    ssao_kernel.emplace_back(sample);
  }

  return ssao_kernel;
}

std::vector<vec3> generate_ssao_noise()
{
  std::uniform_real_distribution<float> random_floats(0.0, 1.0);
  std::default_random_engine generator;
  std::vector<vec3> ssao_noise;

  for (unsigned int i = 0; i < 16; i++) {
    vec3 noise(random_floats(generator) * 2.0f - 1.0f,
               random_floats(generator) * 2.0f - 1.0f,
               0.0f);
    ssao_noise.emplace_back(noise);
  }

  return ssao_noise;
}

SSAO::SSAO(int width, int height, const char* vertex_path, const char* frag_path)
  : fb(width, height, vertex_path, frag_path, { GL_RED }, false),
    dims(width, height)
{
  glGenBuffers(1, &UBO);
  glGenTextures(1, &noise_texture);

  const auto ssao_noise = generate_ssao_noise();

  glBindTexture(GL_TEXTURE_2D, noise_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssao_noise.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);

  const auto ssao_kernel = generate_ssao_kernel();

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, static_cast<long>(ssao_kernel.size() * sizeof (vec4)),
               ssao_kernel.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 6, UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

SSAO::~SSAO()
{
  glDeleteTextures(1, &noise_texture);
  glDeleteBuffers(1, &UBO);
}

void SSAO::bind_framebuffer() const
{
  fb.bind_framebuffer();
}

void SSAO::render() const
{
  fb.shader->use_shader_program();
  glUniform2f(fb.shader->get_uniform_location("dims"), dims.x, dims.y);

  glActiveTexture(GL_TEXTURE28);
  glUniform1i(fb.shader->get_uniform_location("noise"), 28);
  glBindTexture(GL_TEXTURE_2D, noise_texture);

  fb.rect.draw(*fb.shader);

  fb.unbind_framebuffer();
}

void SSAO::use_ssao(const Shader& shader) const
{
  shader.use_shader_program();
  glActiveTexture(GL_TEXTURE29);
  glUniform1i(shader.get_uniform_location("ssao"), 29);
  glBindTexture(GL_TEXTURE_2D, fb.color_textures.front());
}

std::shared_ptr<Shader> SSAO::get_shader() const
{
  return fb.shader;
}
