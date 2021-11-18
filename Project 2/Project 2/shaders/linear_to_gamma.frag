#version 460 core

in vec2 vsUV;

out vec4 fragColor;

uniform sampler2D hdrBuffer;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, vsUV).rgb;

	float gamma = 2.2;
	hdrColor = pow(hdrColor, vec3(1.0 / gamma));

    fragColor = vec4(hdrColor, 1.0);
}