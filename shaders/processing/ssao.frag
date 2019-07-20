# version 450 core
out float frag_color;

in vec2 texture_coords;

uniform sampler2D texture_screen1;
uniform sampler2D texture_screen2;
uniform sampler2D noise;

uniform vec2 dims;

layout (std140, binding = 0) uniform Matrices {
    mat4 perspective;
    mat4 view;
};

layout (std140, binding = 6) uniform SSAO {
    vec3 kernel[64];
};

void main()
{
    vec2 noise_scale = dims / 4.0;
    vec3 frag_pos = texture(texture_screen1, texture_coords).xyz;
    vec3 normal = texture(texture_screen2, texture_coords).rgb;
    vec3 random_vec = texture(noise, texture_coords * noise_scale).xyz;

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    const float radius = 0.5;
    const float bias = 0.025;
    const int kernel_size = 64;

    for (int i = 0; i < kernel_size; i++) {
        vec3 kernel_sample = tbn * kernel[i];
        kernel_sample = frag_pos + kernel_sample * radius;

        vec4 offset = perspective * vec4(kernel_sample, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 - 0.5;

        float sample_depth = texture(texture_screen1, offset.xy).z;
        float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= kernel_sample.z + bias ? 1.0 : 0.0) * range_check;
    }

    frag_color = 1.0 - (occlusion / float(kernel_size));
}
