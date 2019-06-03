#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140, binding = 0) uniform Matrices {
    mat4 world_space;
};

void main()
{
    TexCoords = aPos;
    vec4 pos = world_space * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

