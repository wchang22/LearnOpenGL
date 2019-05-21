#ifndef DATA_H
#define DATA_H

#include <vector>

#include <glm/glm.hpp>

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;

static float cubeVertices[] = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

static float planeVertices[] = {
    // positions          // texture Coords
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

static float transparentVertices[] = {
    // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
    1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

static std::vector<vec3> transparent {
  vec3(-1.5f,  0.0f, -0.48f),
  vec3( 1.5f,  0.0f,  0.51f),
  vec3( 0.0f,  0.0f,  0.7f),
  vec3(-0.3f,  0.0f, -2.3f),
  vec3( 0.5f,  0.0f, -0.6f),
};

static float skyboxVertices[] = {
  // positions
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  -1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f
};

#endif // DATA_H
