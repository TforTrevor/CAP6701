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
uniform float time;

#define PI 3.1415926538

void main()
{
    vsNormal = inNormal;
    vsColor = inColor;
    vsUV = inUV;

    vec3 pos = vec3(inPos.x, inPos.y, inPos.z);

    float amplitude = 0.25;
    float waveLength = 10.0;
    float speed = 1.0;

    float k = 2.0 * PI / waveLength;
    float f = k * (pos.z - speed * time);
    pos.y = amplitude * sin(f);

    vec3 tangent = normalize(vec3(0, k * amplitude * cos(f), 1));
    vec3 normal = vec3(0, tangent.z, -tangent.y);
    vsNormal = normal;

//    vec3 dx = vec3(1, waveHeight * waveFrequency * cos(waveFrequency * inPos.x) * sin(waveFrequency * inPos.z), 0);
//    vec3 dz = vec3(0, waveHeight * waveFrequency * sin(waveFrequency * inPos.x) * cos(waveFrequency * inPos.z), 1);
//
//    vec3 vn = vec3(

//    float dx = 0;
//    float dy = waveHeight * cos(waveFunction);
//
//    vsNormal = normalize(cross(vec3(1, 0, dx), vec3(0, dy, 1)));
//    vsNormal.y = 1.0 - vsNormal.z;
//    vsNormal = normalize(vsNormal);

//    vec3 tangent = normalize(vec3(0.0, waveHeight * cos(waveFunction), 1.0));
//    vec3 normal = vec3(-tangent.y, tangent.x, 0.0);
//
//    vsNormal = normal;

    vsPos = (modelMatrix * vec4(pos, 1.0)).xyz;
}