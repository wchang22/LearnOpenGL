#ifndef DISPLAY_H
#define DISPLAY_H

#include <memory>

#include "shader.h"

class Display {
public:
  Display();
  ~Display();

  void draw() const;

private:
  unsigned int VAO, VBO, EBO;

  static void* buffer_offset(int offset);

  std::unique_ptr<Shader> shaders;
};

#endif // DISPLAY_H
