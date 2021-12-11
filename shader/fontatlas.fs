#version 330 core
// Author: HenryAWE
// License: The 3-clause BSD License

// font atlas shader

in vec2 coord;
out vec4 frag;
uniform sampler2D tex;


void main()
{
    frag.rgb = vec3(1.0f);
    frag.a = texture(tex, vec2(coord.s, 1.0f - coord.t)).r;
}
