#version 330 core
// Author: HenryAWE
// License: The 3-clause BSD License

// Generic 2D rectangle shader

layout (location=0) in vec2 pos;
layout (location=1) in vec2 in_coord;
out vec2 coord;
uniform mat4 matrix;
void main()
{
    gl_Position = matrix * vec4(pos, 0.0f, 1.0f);
    coord = in_coord;
}
