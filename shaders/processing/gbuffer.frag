#version 450 core

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

in V_DATA {
    vec3 position;
    vec2 texture_coords;
    vec3 tangent_light_pos;
    vec3 tangent_view_pos;
    vec3 tangent_frag_pos;
} fs_in;

layout (location = 0) out vec3 position;
layout (location = 1) out vec4 normal_height;
layout (location = 2) out vec4 diffuse_spec;
layout (location = 3) out vec3 tangent_light_pos;
layout (location = 4) out vec3 tangent_view_pos;
layout (location = 5) out vec3 tangent_frag_pos;

uniform bool gamma;

void main() {
    position = fs_in.position;
    normal_height.rgb = texture(texture_normal1, fs_in.texture_coords).rgb;
    if (gamma) {
        normal_height.rgb = pow(normal_height.rgb, vec3(2.2));
    }
    normal_height.rgb = normalize(normal_height.rgb * 2.0 - 1.0);
    normal_height.a = texture(texture_height1, fs_in.texture_coords).r;
    diffuse_spec.rgb = texture(texture_diffuse1, fs_in.texture_coords).rgb;
    diffuse_spec.a = texture(texture_specular1, fs_in.texture_coords).r;
    tangent_light_pos = fs_in.tangent_light_pos;
    tangent_view_pos = fs_in.tangent_view_pos;
    tangent_frag_pos = fs_in.tangent_frag_pos;
}
