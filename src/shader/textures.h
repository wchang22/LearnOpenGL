#ifndef TEXTURES_H
#define TEXTURES_H

#include <vector>
#include <string>

#include "shader/shader.h"

#include <glad/glad.h>

class Textures
{
public:
  Textures();
  ~Textures();
  Textures(Textures&& other) noexcept;

  Textures& operator=(Textures&& other) noexcept;

  void load_texture_from_image(const char* path, std::string_view type);
  void load_cubemap(const std::vector<std::string>& faces);
  void use_textures(const Shader& shader) const;
  void append(Textures&& other);
  void append(const Textures& other);
  size_t size() const;

private:
  std::vector<GLuint> texture_ids;
  std::vector<std::string> texture_paths;
  std::vector<std::string> texture_types;
};

static_assert (std::is_nothrow_move_constructible<Textures>::value, "Mesh not move constructible");
static_assert (std::is_nothrow_move_assignable<Textures>::value, "Mesh not move constructible");

#endif // TEXTURES_H
