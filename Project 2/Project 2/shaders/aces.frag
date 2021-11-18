#version 460 core

in vec2 vsUV;

out vec4 fragColor;

uniform sampler2D hdrBuffer;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, vsUV).rgb;

	float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    vec3 result = clamp((hdrColor * (a * hdrColor + b)) / (hdrColor * (c * hdrColor + d) + e), 0.0, 1.0);

    fragColor = vec4(result, 1.0);
}