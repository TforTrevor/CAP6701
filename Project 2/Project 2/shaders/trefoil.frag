#version 460

in vec3 vsPos;
in vec3 vsNormal;
in vec3 vsColor;
in vec2 vsUV;

out vec4 fragColor;

uniform bool uvMode;
uniform sampler2D rainbowSampler;

void main()
{
	vec3 color = vec3(1, 1, 1);
	color = texture(rainbowSampler, vsUV).rgb;

	if (uvMode)
	{
		color = texture(rainbowSampler, vsUV).rgb;
	}
	else
	{
		ivec2 size = textureSize(rainbowSampler, 0);
		float u = gl_FragCoord.x / size.x;
		float v = gl_FragCoord.y / size.y;
		color = texture(rainbowSampler, vec2(u, v)).rgb;
	}

	fragColor = vec4(color, 1.0);
}