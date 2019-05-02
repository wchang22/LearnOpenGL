#include "camera.h"

#include <GLFW/glfw3.h>

Camera::Camera(vec3 position, vec3 direction, vec3 up)
  : position(position), direction(direction), up(up), time_delta(0.0f), last_frame(0.0f)
{
}

mat4 Camera::lookat() const {
  return glm::lookAt(position, position + direction, up);
}

void Camera::move_forward() {
  position += direction * speed;
}

void Camera::move_backward() {
  position += -direction * speed;
}

void Camera::move_left() {
  position += glm::normalize(glm::cross(up, direction)) * speed;
}

void Camera::move_right() {
  position += glm::normalize(glm::cross(direction, up)) * speed;
}

void Camera::update_frames() {
  float current_frame = static_cast<float>(glfwGetTime());
  time_delta = current_frame - last_frame;
  last_frame = current_frame;
  speed = 2.5f * time_delta;
}
