#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUV;

out vec3 tePos;
out vec3 teNormal;
out vec3 teColor;
out vec2 teUV;

uniform mat4 MVP;
uniform mat4 modelMatrix;

void main()
{
    teNormal = inNormal;
    teColor = inColor;
    teUV = inUV;

    tePos = (modelMatrix * vec4(inPos, 1.0)).xyz;

    gl_Position = MVP * vec4(inPos, 1.0);
}