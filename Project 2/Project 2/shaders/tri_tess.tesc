#version 460 core

struct OutputPatch
{
    vec3 b030;
    vec3 b021;
    vec3 b012;
    vec3 b003;
    vec3 b102;
    vec3 b201;
    vec3 b300;
    vec3 b210;
    vec3 b120;
    vec3 b111;
    vec3 n110;
    vec3 n011;
    vec3 n101;
    vec3 normal[3];
    vec3 color[3];
    vec2 uv[3];
};

layout (vertices = 1) out;

in vec3 vsPos[];
in vec3 vsNormal[];
in vec3 vsColor[];
in vec2 vsUV[];

out patch OutputPatch outPatch;

uniform vec3 viewPos;
uniform int tessLevel;

vec3 b300 = vsPos[0];
vec3 b030 = vsPos[1];
vec3 b003 = vsPos[2];
vec3 n200 = vsNormal[0];
vec3 n020 = vsNormal[1];
vec3 n002 = vsNormal[2];

vec3 projectToPlane(vec3 point, vec3 planePoint, vec3 planeNormal)
{
    vec3 v = point - planePoint;
    float len = dot(v, planeNormal);
    vec3 d = len * planeNormal;
    return (point - d);
}

void calcPositions()
{
    outPatch.b300 = b300;
    outPatch.b030 = b030;
    outPatch.b003 = b003;

    vec3 edgeB300 = outPatch.b003 - outPatch.b030;
    vec3 edgeB030 = outPatch.b300 - outPatch.b003;
    vec3 edgeB003 = outPatch.b030 - outPatch.b300;

    outPatch.b021 = outPatch.b030 + edgeB300 / 3.0;
    outPatch.b012 = outPatch.b030 + edgeB300 * 2.0 / 3.0;
    outPatch.b102 = outPatch.b003 + edgeB030 / 3.0;
    outPatch.b201 = outPatch.b003 + edgeB030 * 2.0 / 3.0;
    outPatch.b210 = outPatch.b300 + edgeB003 / 3.0;
    outPatch.b120 = outPatch.b300 + edgeB003 * 2.0 / 3.0;

    outPatch.b021 = projectToPlane(outPatch.b021, outPatch.b030, n020);
    outPatch.b012 = projectToPlane(outPatch.b012, outPatch.b003, n002);
    outPatch.b102 = projectToPlane(outPatch.b102, outPatch.b003, n002);
    outPatch.b201 = projectToPlane(outPatch.b201, outPatch.b300, n200);
    outPatch.b210 = projectToPlane(outPatch.b210, outPatch.b300, n200);
    outPatch.b120 = projectToPlane(outPatch.b120, outPatch.b030, n020);

    vec3 center = (outPatch.b003 + outPatch.b030 + outPatch.b300) / 3.0;
    outPatch.b111 = (outPatch.b021 + outPatch.b012 + outPatch.b102 + outPatch.b201 + outPatch.b210 + outPatch.b120) / 6.0;
    outPatch.b111 += (outPatch.b111 - center) / 2.0;
}

void calcNormals()
{
    float v12 = 2.0 * dot(b030 - b300, n200 + n020) / dot(b030 - b300, b030 - b300);
    float v23 = 2.0 * dot(b003 - b030, n020 + n002) / dot(b003 - b030, b003 - b030);
    float v31 = 2.0 * dot(b300 - b003, n002 + n200) / dot(b300 - b003, b300 - b003);

    outPatch.n110 = n200 + n020 - v12 * (b030 - b300);
    outPatch.n011 = n020 + n002 - v23 * (b003 - b030);
    outPatch.n101 = n002 + n200 - v31 * (b300 - b003);
}

float map(float value, float min1, float max1, float min2, float max2) 
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    for (int i = 0; i < 3; i++)
    {
        outPatch.normal[i] = vsNormal[i];
        outPatch.color[i] = vsColor[i];
        outPatch.uv[i] = vsUV[i];
    }

    calcPositions();
    calcNormals();

    float viewDistance = distance(viewPos, (vsPos[0] + vsPos[1] + vsPos[2]) / 3.0);

    float remappedDistance = clamp(map(viewDistance, 1, 24, 0, 1), 0, 1);

    float tessellationLevel = mix(tessLevel, 1, remappedDistance);
    gl_TessLevelOuter[0] = tessellationLevel;
    gl_TessLevelOuter[1] = tessellationLevel;
    gl_TessLevelOuter[2] = tessellationLevel;
    gl_TessLevelInner[0] = tessellationLevel;
}