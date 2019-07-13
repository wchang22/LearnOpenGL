#version 450 core

layout (location = 0) in vec3 in_position;

layout (std430, binding = 1) buffer Model {
    mat4 model[];
};


layout (std140, binding = 8) uniform DirectionalShadow {
    mat4 light_space;
};

void main() {
    gl_Position = light_space * model[gl_InstanceID] * vec4(in_position, 1.0);
}
