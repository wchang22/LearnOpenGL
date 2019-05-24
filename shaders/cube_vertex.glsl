#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 position;

uniform mat4 model;

layout (std140, binding = 0) uniform Matrices {
    uniform mat4 view;
    uniform mat4 perspective;
};


void main()
{
    position = vec3(model * vec4(aPos, 1.0));
    normal = inverse(transpose(mat3(model))) * aNormal;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}

