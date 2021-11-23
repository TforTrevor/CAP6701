#version 460 core

layout (location = 0) in vec3 inPos;

out vec3 vsPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
    vsPos = inPos;

    gl_Position =  projectionMatrix * viewMatrix * vec4(vsPos, 1.0);
}