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
    vec3 tangent_view_pos;
    vec3 tangent_frag_pos;
} fs_in;

layout (std140, binding = 0) uniform Matrices {
    mat4 perspective;
    mat4 view;
};

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 position_view;
layout (location = 2) out vec3 normal;
layout (location = 3) out vec3 normal_view;
layout (location = 4) out vec4 diffuse_spec;
layout (location = 5) out vec3 t;
layout (location = 6) out vec3 b;
layout (location = 7) out vec3 n;

uniform bool gamma;
uniform bool parallax;

vec2 parallax_mapping(vec2 texture_coords, vec3 eye_direction) {
    const float height_scale = 0.1;
    const float min_layers = 8;
    const float max_layers = 32;
    const float num_layers = mix(max_layers, min_layers, abs(eye_direction.z));
    const float layer_depth = 1.0 / num_layers;
    const vec2 p = eye_direction.xy * height_scale;
    const vec2 delta_texture_coords = p / num_layers;

    float current_layer_depth = 0.0;
    vec2 current_texture_coords = texture_coords;
    float current_depth_map_value = texture(texture_height1, current_texture_coords).r;

    while (current_layer_depth < current_depth_map_value) {
        current_texture_coords -= delta_texture_coords;
        current_depth_map_value = texture(texture_height1, current_texture_coords).r;
        current_layer_depth += layer_depth;
    }

    vec2 prev_texture_coords = current_texture_coords + delta_texture_coords;
    float prev_layer_depth = current_layer_depth - layer_depth;
    float prev_depth_map_value = texture(texture_height1, prev_texture_coords).r;

    float after_depth_offset = -(current_depth_map_value - current_layer_depth);
    float prev_depth_offset = prev_depth_map_value - prev_layer_depth;
    float weight = after_depth_offset / (after_depth_offset + prev_depth_offset);

    return mix(current_texture_coords, prev_texture_coords, weight);
}

void main() {
    vec2 texture_coords = fs_in.texture_coords;

    if (parallax) {
        vec3 eye_direction = normalize(fs_in.tangent_view_pos - fs_in.tangent_frag_pos);
        texture_coords = parallax_mapping(fs_in.texture_coords, eye_direction);
    }

    position = fs_in.position;
    position_view = vec3(view * vec4(position, 1.0));

    normal = texture(texture_normal1, texture_coords).rgb;

    if (gamma) {
        normal = pow(normal, vec3(2.2));
    }

    normal = normalize(normal * 2.0 - 1.0);
    normal_view = vec3(view * vec4(normal, 0.0));
//    diffuse_spec.rgb = texture(texture_diffuse1, texture_coords).rgb;
//    diffuse_spec.a = texture(texture_specular1, texture_coords).r;
    diffuse_spec = vec4(1.0f);

    t = fs_in.t;
    b = fs_in.b;
    n = fs_in.n;
}
