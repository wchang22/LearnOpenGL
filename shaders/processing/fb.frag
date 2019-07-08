# version 450 core
out vec4 frag_color;

in vec2 texture_coords;

uniform sampler2D hdr;
uniform sampler2D bloom;

vec3 reinhard_tone_mapping(vec3 color) {
    return color / (color + vec3(1.0));
}

void main()
{
    vec3 color = texture(hdr, texture_coords).rgb +
                 texture(bloom, texture_coords).rgb;

    frag_color = vec4(reinhard_tone_mapping(color), 1.0);
}
