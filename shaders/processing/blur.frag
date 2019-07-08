# version 450 core
out vec4 frag_color;

in vec2 texture_coords;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    const float tex_span = 4.0;
    const int num_samples = 5;
    const vec2 tex_offset_size = tex_span / textureSize(image, 0);
    vec3 color = texture(image, texture_coords).rgb * weight[0];

    if (horizontal) {
        for (int i = 1; i < num_samples; i++) {
            vec2 tex_offset = vec2(tex_offset_size.x * i, 0.0);

            color += texture(image, texture_coords + tex_offset).rgb * weight[i];
            color += texture(image, texture_coords - tex_offset).rgb * weight[i];
        }
    } else {
        for (int i = 1; i < num_samples; i++) {
            vec2 tex_offset = vec2(0.0, tex_offset_size.y * i);

            color += texture(image, texture_coords + tex_offset).rgb * weight[i];
            color += texture(image, texture_coords - tex_offset).rgb * weight[i];
        }
    }

    frag_color = vec4(color, 1.0);
}
