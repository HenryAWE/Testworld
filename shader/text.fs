#version 330 core
// Author: HenryAWE
// License: The 3-clause BSD License

// Text shader

in vec2 coord;
flat in int tex_id;
out vec4 frag;
uniform sampler2D tex[16];


void main()
{
    frag.rgb = vec3(1.0f);
    frag.a = texture(tex[tex_id], coord).r;
}
