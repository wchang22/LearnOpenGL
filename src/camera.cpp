#include "camera.h"
#include "window.h"

#include <algorithm>

#include <GLFW/glfw3.h>

const float SPEED_MULTIPLIER = 2.5f;

Camera::Camera(vec3 position, vec3 forward, vec3 up)
  : position(position),
    forward(forward),
    up(up),
    speed(0.0f),
    time_delta(0.0f),
    last_frame(0.0f),
    pitch(0.0f),
    yaw(270.0f),
    fovy(45.0f)
{
}

mat4 Camera::lookat() const {
  return glm::lookAt(position, position + forward, up);
}

mat4 Camera::perspective() const {
  return glm::perspective(glm::radians(fovy), static_cast<float>(Window::WIDTH) / Window::HEIGHT,
                          0.1f, 100.0f);
}

void Camera::move(Direction direction) {
  switch (direction) {
    case FORWARD:
      position += forward * speed;
      break;
    case BACKWARD:
      position += -forward * speed;
      break;
    case LEFT:
      position += glm::normalize(glm::cross(up, forward)) * speed;
      break;
    case RIGHT:
      position += glm::normalize(glm::cross(forward, up)) * speed;
      break;
    case UP:
      position += up * speed;
      break;
    case DOWN:
      position += -up * speed;
      break;
  }
}

void Camera::update_frames() {
  float current_frame = static_cast<float>(glfwGetTime());
  time_delta = current_frame - last_frame;
  last_frame = current_frame;
  speed = SPEED_MULTIPLIER * time_delta;
}

void Camera::update_direction(float delta_x, float delta_y) {
  yaw += delta_x;
  pitch = std::clamp(pitch + delta_y, -89.0f, 89.0f);

  forward.x = static_cast<float>(cos(static_cast<double>(glm::radians(pitch))) *
                                   cos(static_cast<double>(glm::radians(yaw))));
  forward.y = static_cast<float>(sin(static_cast<double>(glm::radians(pitch))));
  forward.z = static_cast<float>(cos(static_cast<double>(glm::radians(pitch))) *
                                   sin(static_cast<double>(glm::radians(yaw))));
  forward = glm::normalize(forward);
}

void Camera::update_fov(float delta_y) {
  fovy = std::clamp(fovy - delta_y, 1.0f, 45.0f);
}

vec3 Camera::get_position() const {
  return position;
}

vec3 Camera::get_direction() const {
  return forward;
}
