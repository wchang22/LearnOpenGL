#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;

class Camera
{
public:
  Camera(vec3 position, vec3 forward, vec3 up);

  enum class Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
  };

  mat4 lookat() const;
  mat4 perspective() const;
  void move(Direction direction);
  void update_frames();
  void update_direction(float delta_x, float delta_y);
  void update_fov(float delta_y);
  vec3 get_position() const;
  vec3 get_direction() const;

private:
  vec3 up;
  vec3 position;
  vec3 forward;

  float speed;
  float time_delta;
  float last_frame;
  float pitch;
  float yaw;
  float fovy;
};

#endif // CAMERA_H
