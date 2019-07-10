# version 450 core

layout (location = 0) in vec3 in_position;

layout (std140, binding = 0) uniform Matrices {
    mat4 perspective;
    mat4 view;
    mat4 model;
};

void main() {
    gl_Position = perspective * view * model * vec4(in_position, 1.0);
}
