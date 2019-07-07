#version 450 core

layout (location = 0) in vec3 in_position;

layout (std140, binding = 0) uniform Matrices {
    mat4 world_space;
    mat4 model;
};

layout (std140, binding = 8) uniform DirectionalShadow {
    mat4 light_space;
};

void main() {
    gl_Position = light_space * model * vec4(in_position, 1.0);
}
