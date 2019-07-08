#version 450 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texture_coords;

out vec2 texture_coords;

void main()
{
    gl_Position = vec4(in_position.x, in_position.y, 0.0, 1.0);
    texture_coords = in_texture_coords;
}
