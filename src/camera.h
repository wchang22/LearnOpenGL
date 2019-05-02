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
  mat4 perspective() const;
  void move_forward();
  void move_backward();
  void move_left();
  void move_right();
  void update_frames();
  void update_direction(float delta_x, float delta_y);
  void update_fov(float delta_y);

private:

  vec3 position;
  vec3 direction;
  vec3 up;

  float speed;
  float time_delta;
  float last_frame;
  float pitch;
  float yaw;
  float fovy;
};

#endif // CAMERA_H
