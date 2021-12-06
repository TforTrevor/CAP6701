#version 460

#include "common.glsl"

//in vec2 vsUV;
in vec4 near4;
in vec4 far4;

out vec4 fragColor;

uniform vec3 viewPos;
uniform bool showSun;

uniform sampler2D transmittanceSampler;
uniform sampler2D skyViewSampler;

vec3 getValFromSkyLUT(vec3 rayDir, vec3 sunDir) 
{
    float height = length(viewPos);
    vec3 up = viewPos / height;
    
    float horizonAngle = safeacos(sqrt(height * height - groundRadius * groundRadius) / height);
    float altitudeAngle = horizonAngle - acos(dot(rayDir, up)); // Between -PI/2 and PI/2
    float azimuthAngle; // Between 0 and 2*PI
    if (abs(altitudeAngle) > (0.5*PI - .0001)) 
    {
        // Looking nearly straight up or down.
        azimuthAngle = 0.0;
    } 
    else 
    {
        vec3 right = cross(sunDir, up);
        vec3 forward = cross(up, right);
        
        vec3 projectedDir = normalize(rayDir - up*(dot(rayDir, up)));
        float sinTheta = dot(projectedDir, right);
        float cosTheta = dot(projectedDir, forward);
        azimuthAngle = atan(sinTheta, cosTheta) + PI;
    }
    
    // Non-linear mapping of altitude angle. See Section 5.3 of the paper.
    float v = 0.5 + 0.5*sign(altitudeAngle)*sqrt(abs(altitudeAngle)*2.0/PI);
    vec2 uv = vec2(azimuthAngle / (2.0*PI), v);
    uv *= textureSize(skyViewSampler, 0);
    uv /= textureSize(skyViewSampler, 0);
    
    return texture(skyViewSampler, uv).rgb;
}

vec3 sunWithBloom(vec3 rayDir, vec3 sunDir) 
{
    const float sunSolidAngle = 0.53*PI/180.0;
    const float minSunCosTheta = cos(sunSolidAngle);

    float cosTheta = dot(rayDir, sunDir);
    if (cosTheta >= minSunCosTheta) return vec3(1.0);
    
    float offset = minSunCosTheta - cosTheta;
    float gaussianBloom = exp(-offset*50000.0)*0.5;
    float invBloom = 1.0/(0.02 + offset*300.0)*0.01;
    return vec3(gaussianBloom+invBloom);
}

void main()
{
    vec3 origin = near4.xyz/ near4.w;
    vec3 far3 = far4.xyz / far4.w;
    vec3 rayDir = far3 - origin;
    rayDir = normalize(rayDir);

	//vec3 sunDir = getSunDir();
    
    vec3 lum = getValFromSkyLUT(rayDir, -sunDirection);

    if (showSun)
    {
        // Bloom should be added at the end, but this is subtle and works well.
        vec3 sunLum = sunWithBloom(rayDir, -sunDirection);
        // Use smoothstep to limit the effect, so it drops off to actual zero.
        sunLum = smoothstep(0.002, 1.0, sunLum);
        if (length(sunLum) > 0.0) 
        {
            if (rayIntersectSphere(viewPos, rayDir, groundRadius) >= 0.0) 
            {
                sunLum *= 0.0;
            } 
            else 
            {
                // If the sun value is applied to this pixel, we need to calculate the transmittance to obscure it.
                sunLum *= getValFromTLUT(transmittanceSampler, textureSize(transmittanceSampler, 0), viewPos, -sunDirection);
            }
        }
        lum += sunLum;
    }    

    lum *= 5;
    
    fragColor = vec4(lum,1.0);
}