#version 460 core

struct OutputPatch
{
    vec3 b0;
    vec3 b01;
    vec3 b10;
    vec3 b1;
    vec3 b12;
    vec3 b21;
    vec3 b2;
    vec3 b23;
    vec3 b32;
    vec3 b3;
    vec3 b30;
    vec3 b03;
    vec3 b02;
    vec3 b13;
    vec3 b20;
    vec3 b31;
    vec3 n0;
    vec3 n01;
    vec3 n1;
    vec3 n12;
    vec3 n2;
    vec3 n23;
    vec3 n3;
    vec3 n30;
    vec3 n0123;
    vec2 uv[4];
};

layout (vertices = 1) out;

in vec3 vsPos[];
in vec3 vsNormal[];
in vec3 vsColor[];
in vec2 vsUV[];

out patch OutputPatch outPatch;

uniform vec3 viewPos;
uniform int tessLevel;

vec3 b0 = vsPos[0];
vec3 b1 = vsPos[1];
vec3 b2 = vsPos[2];
vec3 b3 = vsPos[3];
vec3 n0 = vsNormal[0];
vec3 n1 = vsNormal[1];
vec3 n2 = vsNormal[2];
vec3 n3 = vsNormal[3];

void calcPositions()
{
    outPatch.b0 = b0;
    outPatch.b1 = b1;
    outPatch.b2 = b2;
    outPatch.b3 = b3;

    outPatch.b01 = (2.0 * b0 + b1 - (dot(b1 - b0, n0) * n0)) / 3.0;
    outPatch.b10 = (2.0 * b1 + b0 - (dot(b0 - b1, n1) * n1)) / 3.0;
    outPatch.b12 = (2.0 * b1 + b2 - (dot(b2 - b1, n1) * n1)) / 3.0;
    outPatch.b21 = (2.0 * b2 + b1 - (dot(b1 - b2, n2) * n2)) / 3.0;
    outPatch.b23 = (2.0 * b2 + b3 - (dot(b3 - b2, n2) * n2)) / 3.0;
    outPatch.b32 = (2.0 * b3 + b2 - (dot(b2 - b3, n3) * n3)) / 3.0;
    outPatch.b30 = (2.0 * b3 + b0 - (dot(b0 - b3, n3) * n3)) / 3.0;
    outPatch.b03 = (2.0 * b0 + b3 - (dot(b3 - b0, n0) * n0)) / 3.0;

    vec3 q = outPatch.b01 + outPatch.b10 + outPatch.b12 + outPatch.b21 + outPatch.b23 + outPatch.b32 + outPatch.b30 + outPatch.b03;

    vec3 e0 = (2.0 * (outPatch.b01 + outPatch.b03 + q) - (outPatch.b21 + outPatch.b23)) / 18.0;
    vec3 e1 = (2.0 * (outPatch.b12 + outPatch.b10 + q) - (outPatch.b32 + outPatch.b30)) / 18.0;
    vec3 e2 = (2.0 * (outPatch.b23 + outPatch.b21 + q) - (outPatch.b03 + outPatch.b01)) / 18.0;
    vec3 e3 = (2.0 * (outPatch.b30 + outPatch.b32 + q) - (outPatch.b10 + outPatch.b12)) / 18.0;

    vec3 v0 = (4.0 * b0 + 2.0 * (b3 + b1) + b2) / 9.0;
    vec3 v1 = (4.0 * b1 + 2.0 * (b0 + b2) + b3) / 9.0;
    vec3 v2 = (4.0 * b2 + 2.0 * (b1 + b3) + b0) / 9.0;
    vec3 v3 = (4.0 * b3 + 2.0 * (b2 + b0) + b1) / 9.0;

    outPatch.b02 = e0 + (e0 - v0) / 2.0;
    outPatch.b13 = e1 + (e1 - v1) / 2.0;
    outPatch.b20 = e2 + (e2 - v2) / 2.0;
    outPatch.b31 = e3 + (e3 - v3) / 2.0;
}

void calcNormals()
{
    outPatch.n0 = n0;
    outPatch.n1 = n1;
    outPatch.n2 = n2;
    outPatch.n3 = n3;

    float v01 = 2.0 * dot(b1 - b0, n0 + n1) / dot(b1 - b0, b1 - b0);
    float v12 = 2.0 * dot(b2 - b1, n1 + n2) / dot(b2 - b1, b2 - b1);
    float v23 = 2.0 * dot(b3 - b2, n2 + n3) / dot(b3 - b2, b3 - b2);
    float v30 = 2.0 * dot(b0 - b3, n3 + n0) / dot(b0 - b3, b0 - b3);

    outPatch.n01 = n0 + n1 - v01 * (b1 - b0);
    outPatch.n12 = n1 + n2 - v12 * (b2 - b1);
    outPatch.n23 = n2 + n3 - v23 * (b3 - b2);
    outPatch.n30 = n3 + n0 - v30 * (b0 - b3);

    outPatch.n0123 = ((2.0 * (outPatch.n01 + outPatch.n12 + outPatch.n23 + outPatch.n30)) + (n0 + n1 + n2 + n3)) / 12.0;
}

float map(float value, float min1, float max1, float min2, float max2) 
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    for (int i = 0; i < 4; i++)
    {
        //outPatch.color[i] = vsColor[i];
        outPatch.uv[i] = vsUV[i];
    }

    calcPositions();
    calcNormals();

    float viewDistance = distance(viewPos, (vsPos[0] + vsPos[1] + vsPos[2] + vsPos[3]) / 4.0);

    float remappedDistance = clamp(map(viewDistance, 1, 24, 0, 1), 0, 1);

    float tessellationLevel = mix(tessLevel, 1, remappedDistance);
    gl_TessLevelOuter[0] = tessellationLevel;
    gl_TessLevelOuter[1] = tessellationLevel;
    gl_TessLevelOuter[2] = tessellationLevel;
    gl_TessLevelInner[0] = tessellationLevel;
    gl_TessLevelInner[1] = tessellationLevel;
}