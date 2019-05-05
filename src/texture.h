#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>

#include <glad/glad.h>

class Texture
{
public:
  Texture();
  ~Texture();

  void load_texture_from_image(const char* path, GLenum image_format);
  void use_textures();

private:
  std::vector<GLuint> textures;
};

#endif // TEXTURE_H
