#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
out vec2 texFrag;
out vec4 posclip;
void main()
{
    gl_Position = vec4(position, 1.0f);
    texFrag = texCoord;
}