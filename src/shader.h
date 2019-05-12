#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader {
public:
  Shader(const char* path_vertex, const char* path_fragment);
  ~Shader();

  void use_shader_program();
  int get_uniform_location(const char* uniform) const;

private:
  static std::string read_source(const char* path);
  static bool check_shader_errors(unsigned int shader);
  static bool check_program_errors(unsigned int program);

  unsigned int vertex_shader;
  unsigned int fragment_shader;
  unsigned int shader_program;
};

#endif // SHADER_H
