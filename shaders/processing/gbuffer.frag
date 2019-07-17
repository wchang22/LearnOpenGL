#version 450 core

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

in V_DATA {
    vec3 position;
    vec2 texture_coords;
    vec3 t;
    vec3 b;
    vec3 n;
} fs_in;

layout (location = 0) out vec3 position;
layout (location = 1) out vec4 normal_height;
layout (location = 2) out vec4 diffuse_spec;
layout (location = 3) out vec3 t;
layout (location = 4) out vec3 b;
layout (location = 5) out vec3 n;

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
    t = fs_in.t;
    b = fs_in.b;
    n = fs_in.n;
}
