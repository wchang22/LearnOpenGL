#ifndef TEXTURES_H
#define TEXTURES_H

#include <vector>
#include <unordered_set>

#include "shader/shader.h"

#include <glad/glad.h>

class Textures
{
public:
  Textures() = default;
  ~Textures();
  Textures(Textures&& other) noexcept;

  Textures& operator=(Textures&& other) noexcept;

  void load_texture_from_image(std::string_view path, std::string_view type);
  void load_cubemap(std::initializer_list<const char*> faces);
  void add_texture(std::string_view type, unsigned int id);
  void use_textures(const Shader& shader) const;
  void append(Textures&& other);
  void append(const Textures& other);
  void clear();
  size_t size() const;

private:
  std::vector<unsigned int> texture_ids;
  std::vector<std::string> texture_paths;
  std::vector<std::string> texture_types;
  std::unordered_set<unsigned int> external_ids;
};

static_assert (std::is_nothrow_move_constructible<Textures>::value, "Mesh not move constructible");
static_assert (std::is_nothrow_move_assignable<Textures>::value, "Mesh not move constructible");

#endif // TEXTURES_H
