#version 330 core
// Author: HenryAWE
// License: The 3-clause BSD License

// screen texture shader

in vec2 coord;
out vec4 frag;
uniform sampler2D tex;

vec3 Grayscale(vec3 col)
{
    float average = 0.2126f * col.r + 0.7152f * col.g + 0.0722f * col.b;
    return vec3(average);
}

void main()
{
    frag = texture(tex, coord);
    frag.rgb = Grayscale(frag.rgb);
}
