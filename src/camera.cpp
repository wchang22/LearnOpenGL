#include "camera.h"
#include "window.h"

#include <algorithm>
#include <iostream>

#include <GLFW/glfw3.h>

const float SPEED_MULTIPLIER = 2.5f;

Camera::Camera(vec3 position, vec3 direction, vec3 up)
  : position(position),
    direction(direction),
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
  return glm::lookAt(position, position + direction, up);
}

mat4 Camera::perspective() const {
  return glm::perspective(glm::radians(fovy), static_cast<float>(Window::WIDTH) / Window::HEIGHT,
                          0.1f, 100.0f);
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

void Camera::move_up() {
  position += up * speed;
}

void Camera::move_down() {
  position += -up * speed;
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

  direction.x = static_cast<float>(cos(static_cast<double>(glm::radians(pitch))) *
                                   cos(static_cast<double>(glm::radians(yaw))));
  direction.y = static_cast<float>(sin(static_cast<double>(glm::radians(pitch))));
  direction.z = static_cast<float>(cos(static_cast<double>(glm::radians(pitch))) *
                                   sin(static_cast<double>(glm::radians(yaw))));
  direction = glm::normalize(direction);
}

void Camera::update_fov(float delta_y) {
  fovy = std::clamp(fovy - delta_y, 1.0f, 45.0f);
}
