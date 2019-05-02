#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;

class Camera
{
public:
  Camera(vec3 position, vec3 direction, vec3 up);

  mat4 lookat() const;
  void move_forward();
  void move_backward();
  void move_left();
  void move_right();
  void update_frames();

private:

  vec3 position;
  vec3 direction;
  vec3 up;

  float speed = 0.05f;
  float time_delta;
  float last_frame;

};

#endif // CAMERA_H
