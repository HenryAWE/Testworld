#version 330 core
// Author: HenryAWE
// License: The 3-clause BSD License

// Text shader

layout (location=0) in vec2 pos;
layout (location=1) in vec2 in_coord;
layout (location=2) in int in_tex_id;
out vec2 coord;
flat out int tex_id;
uniform mat4 matrix;
void main()
{
    gl_Position = matrix * vec4(pos, 0.0f, 1.0f);
    coord = in_coord;
    tex_id = in_tex_id;
}
