#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 normal;
out vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main() {
    gl_Position = perspective * view * model * vec4(in_position, 1.0);
    position = in_position;
    normal = in_normal;
}
