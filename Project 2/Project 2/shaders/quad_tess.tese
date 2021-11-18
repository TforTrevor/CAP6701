#version 460 core

layout(quads, equal_spacing, ccw) in;

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

in patch OutputPatch outPatch;

out vec3 tePos;
out vec3 teNormal;
out vec3 teColor;
out vec2 teUV;

uniform mat4 vpMatrix;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    float bu0 = (1.0 - u) * (1.0 - u) * (1.0 - u);
    float bu1 = 3.0 * u * (1.0 - u) * (1.0 - u);
    float bu2 = 3.0 * u * u * (1.0 - u);
    float bu3 = u * u * u;

    float bv0 = (1.0 - v) * (1.0 - v) * (1.0 - v);
    float bv1 = 3.0 * v * (1.0 - v) * (1.0 - v);
    float bv2 = 3.0 * v * v * (1.0 - v);
    float bv3 = v * v * v;

    tePos = bu0 * (bv0 * outPatch.b0 + bv1 * outPatch.b01 + bv2 * outPatch.b10 + bv3 * outPatch.b1) +
            bu1 * (bv0 * outPatch.b03 + bv1 * outPatch.b02 + bv2 * outPatch.b13 + bv3 * outPatch.b12) +
            bu2 * (bv0 * outPatch.b30 + bv1 * outPatch.b31 + bv2 * outPatch.b20 + bv3 * outPatch.b21) +
            bu3 * (bv0 * outPatch.b3 + bv1 * outPatch.b32 + bv2 * outPatch.b23 + bv3 * outPatch.b2);

    float nu0 = (1.0 - u) * (1.0 - u);
    float nu1 = 2.0 * u * (1.0 - u);
    float nu2 = u * u;

    float nv0 = (1.0 - v) * (1.0 - v);
    float nv1 = 2.0 * v * (1.0 - v);
    float nv2 = v * v;

    teNormal = nu0 * (nv0 * outPatch.n0 + nv1 * outPatch.n01 + nv2 * outPatch.n1) +
               nu1 * (nv0 * outPatch.n30 + nv1 * outPatch.n0123 + nv2 * outPatch.n12) +
               nu2 * (nv0 * outPatch.n3 + nv1 * outPatch.n23 + nv2 * outPatch.n2);

    float tu0 = 1.0 - u;
    float tu1 = u;
    float tv0 = 1.0 - v;
    float tv1 = v;

    teUV = tu0 * (tv0 * outPatch.uv[0] + tv1 * outPatch.uv[1]) +
           tu1 * (tv0 * outPatch.uv[3] + tv1 * outPatch.uv[2]);

//    teColor = tu0 * (tv0 * outPatch.color[0] + tv1 * outPatch.color[1]) +
//              tu1 * (tv0 * outPatch.color[3] + tv1 * outPatch.color[2]);

    teColor = vec3(1.0, 1.0, 1.0);

//    tePos = tu0 * (tv0 * outPatch.b0 + tv1 * outPatch.b1) +
//           tu1 * (tv0 * outPatch.b2 + tv1 * outPatch.b3);

    gl_Position = vpMatrix * vec4(tePos, 1.0);
}