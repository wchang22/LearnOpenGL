#ifndef TEXTURES_H
#define TEXTURES_H

#include <vector>
#include <string>

#include "shader.h"

#include <glad/glad.h>

class Textures
{
public:
  Textures();
  ~Textures();
  Textures(Textures&& other);

  void load_texture_from_image(const char* path, const std::string& type);
  void load_cubemap(const std::vector<std::string>& faces);
  void use_textures(const Shader& shader) const;
  void append(Textures&& other);
  void append(const Textures& other);

private:
  std::vector<GLuint> texture_ids;
  std::vector<std::string> texture_paths;
  std::vector<std::string> texture_types;
};

#endif // TEXTURES_H
