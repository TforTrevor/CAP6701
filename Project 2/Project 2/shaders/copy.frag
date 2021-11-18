#version 460 core

in vec2 vsUV;

out vec4 fragColor;

uniform sampler2D hdrBuffer;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, vsUV).rgb;

    fragColor = vec4(hdrColor, 1.0);
}