#ifndef SHADER_H
#define SHADER_H

#include <stdexcept>

class Shader {
public:
  Shader(const char* path_vertex, const char* path_fragment);
  ~Shader();

  struct ShaderException : public std::runtime_error {
    ShaderException(const char* msg);
    const char* what() const noexcept override;
  };

  void use_shader() const;

private:
  std::string read_source(const char* path);
  bool check_shader_errors(unsigned int shader);
  bool check_program_errors(unsigned int program);

  unsigned int vertex_shader;
  unsigned int fragment_shader;
  unsigned int shader_program;
};

#endif // SHADER_H
