#version 460 core

in vec2 vsUV;

out vec4 fragColor;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, vsUV).rgb;

	vec3 result = hdrColor * exposure;
	fragColor = vec4(result, 1.0);
}