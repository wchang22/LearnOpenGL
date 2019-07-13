#version 450 core

layout (location = 0) in vec3 in_position;

layout (std430, binding = 1) buffer Model {
    mat4 model[];
};

void main() {
    gl_Position = model[gl_InstanceID] * vec4(in_position, 1.0);
}
