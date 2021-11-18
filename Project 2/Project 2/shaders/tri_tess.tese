#version 460 core

layout(triangles, equal_spacing, ccw) in;

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

in patch OutputPatch outPatch;

out vec3 tePos;
out vec3 teNormal;
out vec3 teColor;
out vec2 teUV;

uniform mat4 vpMatrix;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    //teNormal = interpolate3D(outPatch.normal[0], outPatch.normal[1], outPatch.normal[2]);
    teColor = interpolate3D(outPatch.color[1], outPatch.color[2], outPatch.color[0]);
    teUV = interpolate2D(outPatch.uv[1], outPatch.uv[2], outPatch.uv[0]);

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    float uPow3 = pow(u, 3);
    float vPow3 = pow(v, 3);
    float wPow3 = pow(w, 3);
    float uPow2 = pow(u, 2);
    float vPow2 = pow(v, 2);
    float wPow2 = pow(w, 2);

    tePos = outPatch.b300 * wPow3 +
            outPatch.b030 * uPow3 +
            outPatch.b003 * vPow3 +
            outPatch.b210 * 3.0 * wPow2 * u +
            outPatch.b120 * 3.0 * w * uPow2 +
            outPatch.b201 * 3.0 * wPow2 * v +
            outPatch.b021 * 3.0 * uPow2 * v +
            outPatch.b102 * 3.0 * w * vPow2 +
            outPatch.b012 * 3.0 * u * vPow2 +
            outPatch.b111 * 6.0 * w * u * v;

    vec3 n200 = outPatch.normal[0];
    vec3 n020 = outPatch.normal[1];
    vec3 n002 = outPatch.normal[2];    

    teNormal =  n200 * wPow2 +
                n020 * uPow2 +
                n002 * vPow2 +
                outPatch.n110 * w * u +
                outPatch.n011 * u * v +
                outPatch.n101 * w * v;

    teNormal = normalize(teNormal);

    gl_Position = vpMatrix * vec4(tePos, 1.0);
}