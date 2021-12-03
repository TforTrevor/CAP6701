#version 460

#include common.glsl

in vec2 vsUV;

out vec4 fragColor;

uniform vec3 sunDirection;
uniform int stepCount;

vec3 getSunTransmittance(vec3 pos, vec3 sunDir)
{
	if (rayIntersectSphere(pos, sunDir, groundRadius) > 0.0)
	{
		return vec3(0.0);
	}

	float atmosphereDistance = rayIntersectSphere(pos, sunDir, atmosphereRadius);
	float t = 0.0;

	vec3 transmittance = vec3(1.0);
	for (int i = 0; i < stepCount; i++)
	{
		float newT = ((i + 0.3) / stepCount) * atmosphereDistance;
		float dt = newT - t;
		t = newT;

		vec3 newPos = pos + t * sunDir;

		vec3 rayleighScattering;
		vec3 extinction;
		float mieScattering;
		getScatteringValues(newPos, rayleighScattering, mieScattering, extinction);

		transmittance *= exp(-dt * extinction);
	}

	return transmittance;
}

void main()
{
	float u = clamp(gl_FragCoord.x, 0.0, lutRes.x - 1.0) / lutRes.x;
	float v = clamp(gl_FragCoord.y, 0.0, lutRes.y - 1.0) / lutRes.y;

	float sunCosTheta = 2.0 * u - 1.0;
	float sunTheta = safeacos(sunCosTheta);
	float height = mix(groundRadius, atmosphereRadius, v);

	vec3 pos = vec3(0.0, height, 0.0);
	vec3 sunDir = normalize(vec3(0.0, sunCosTheta, -sin(sunTheta)));

	fragColor = vec4(getSunTransmittance(pos, sunDir), 1.0);
}