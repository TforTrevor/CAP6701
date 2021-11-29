#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUV;

out vec3 vsPos;
out vec3 vsNormal;
out vec3 vsColor;
out vec2 vsUV;

uniform mat4 MVP;
uniform mat4 modelMatrix;

void main()
{
    vsNormal = inNormal;
    vsColor = inColor;
    vsUV = inUV;

    vsPos = (modelMatrix * vec4(inPos, 1.0)).xyz;

    gl_Position = MVP * vec4(inPos, 1.0);
}