#version 330 core
// Author: HenryAWE
// License: The 3-clause BSD License

// screen texture shader

in vec2 coord;
out vec4 frag;
uniform sampler2D tex;
uniform sampler2D perlin;
uniform int use_perlin;
uniform float off_factor;

vec3 Grayscale(vec3 col)
{
    float average = 0.2126f * col.r + 0.7152f * col.g + 0.0722f * col.b;
    return vec3(average);
}

void main()
{
    vec2 offset = vec2(0.0f);
    if(use_perlin == 1)
        offset = vec2(texture(perlin, coord).r - 0.5f) * off_factor;
    frag = texture(tex, coord + offset);
    frag.rgb = (Grayscale(frag.rgb) + frag.rgb) / 2.0f;
}
