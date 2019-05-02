#version 450 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texture_coord;

out vec2 vertex_texture_coord;

uniform mat4 modelview;
uniform mat4 perspective;

void main() {
    gl_Position = perspective * modelview * vec4(position, 1.0);
    vertex_texture_coord = texture_coord;
}
