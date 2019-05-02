#version 450 core

in vec2 vertex_texture_coord;

out vec4 frag_color;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
    vec4 textures = mix(texture(texture0, vertex_texture_coord),
                        texture(texture1, vertex_texture_coord),
                        0.4);
    frag_color = textures;
}
