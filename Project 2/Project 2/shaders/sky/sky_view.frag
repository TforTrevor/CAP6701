#version 460

#include "common.glsl"

in vec2 vsUV;

out vec4 fragColor;

//uniform vec3 sunDirection;
uniform int stepCount;
uniform vec3 viewPos;

uniform sampler2D transmittanceSampler;
uniform sampler2D multiScatterSampler;

vec3 raymarchScattering(vec3 pos, vec3 rayDir, vec3 sunDir, float tMax, int numSteps)
{
	float cosTheta = dot(rayDir, sunDir);
    
	float miePhaseValue = getMiePhase(cosTheta);
	float rayleighPhaseValue = getRayleighPhase(-cosTheta);
    
    vec3 lum = vec3(0.0);
    vec3 transmittance = vec3(1.0);
    float t = 0.0;
    for (int i = 0; i < numSteps; i++) 
    {
        float newT = ((i + 0.3)/numSteps)*tMax;
        float dt = newT - t;
        t = newT;
        
        vec3 newPos = pos + t*rayDir;
        
        vec3 rayleighScattering, extinction;
        float mieScattering;
        getScatteringValues(newPos, rayleighScattering, mieScattering, extinction);
        
        vec3 sampleTransmittance = exp(-dt*extinction);

        vec3 sunTransmittance = getValFromTLUT(transmittanceSampler, textureSize(transmittanceSampler, 0), newPos, sunDir);
        vec3 psiMS = getValFromMultiScattLUT(multiScatterSampler, textureSize(multiScatterSampler, 0), newPos, sunDir);
        
        vec3 rayleighInScattering = rayleighScattering*(rayleighPhaseValue*sunTransmittance + psiMS);
        vec3 mieInScattering = mieScattering*(miePhaseValue*sunTransmittance + psiMS);
        vec3 inScattering = (rayleighInScattering + mieInScattering);

        // Integrated scattering within path segment.
        vec3 scatteringIntegral = (inScattering - inScattering * sampleTransmittance) / extinction;

        lum += scatteringIntegral*transmittance;
        
        transmittance *= sampleTransmittance;
    }
    return lum;
}

void main()
{
    float u = clamp(gl_FragCoord.x, 0.0, lutRes.x - 1.0) / lutRes.x;
	float v = clamp(gl_FragCoord.y, 0.0, lutRes.y - 1.0) / lutRes.y;

	float azimuthAngle = (u - 0.5)*2.0*PI;
    // Non-linear mapping of altitude. See Section 5.3 of the paper.
    float adjV;
    if (v < 0.5) 
    {
		float coord = 1.0 - 2.0*v;
		adjV = -coord*coord;
	} 
    else 
    {
		float coord = v*2.0 - 1.0;
		adjV = coord*coord;
	}
    
    float height = length(viewPos);
    vec3 up = viewPos / height;
    float horizonAngle = safeacos(sqrt(height * height - groundRadius * groundRadius) / height) - 0.5*PI;
    float altitudeAngle = adjV*0.5*PI - horizonAngle;
    
    float cosAltitude = cos(altitudeAngle);
    vec3 rayDir = vec3(cosAltitude*sin(azimuthAngle), sin(altitudeAngle), -cosAltitude*cos(azimuthAngle));

    float sunAltitude = (0.5 * PI) - acos(dot(getSunDir(), up));
    vec3 sunDir = vec3(0.0, sin(sunAltitude), -cos(sunAltitude));
    
    float atmoDist = rayIntersectSphere(viewPos, rayDir, atmosphereRadius);
    float groundDist = rayIntersectSphere(viewPos, rayDir, groundRadius);
    float tMax = (groundDist < 0.0) ? atmoDist : groundDist;
    vec3 lum = raymarchScattering(viewPos, rayDir, sunDir, tMax, stepCount);

    fragColor = vec4(lum, 1.0);
}