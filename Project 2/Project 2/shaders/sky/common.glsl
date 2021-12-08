uniform vec2 lutRes;

//Atmosphere Properties
uniform float groundRadius = 6.360;
uniform float atmosphereRadius = 6.460;

uniform vec3 rayleighScatteringBase = vec3(5.802, 13.558, 33.100);
uniform float rayleighAbsorptionBase = 0.0;

uniform float meiScatteringBase = 3.996;
uniform float mieAbsorptionBase = 4.400;

uniform vec3 ozoneAbsorptionBase = vec3(0.650, 1.881, 0.085);

uniform vec3 groundAlbedo = vec3(0.3, 0.3, 0.3);

const float PI = 3.14159265358;

//float getSunAltitude()
//{
//    const float periodSec = 120.0;
//    const float halfPeriod = periodSec / 2.0;
//    const float sunriseShift = 0.1;
//    float cyclePoint = (1.0 - abs((mod(time, periodSec) - halfPeriod) / halfPeriod));
//    cyclePoint = (cyclePoint * (1.0 + sunriseShift)) - sunriseShift;
//
//    return (0.5 * PI) * cyclePoint;
//}
//
//vec3 getSunDir()
//{
//    float altitude = getSunAltitude();
//    return normalize(vec3(0.0, sin(altitude), -cos(altitude)));
//}

float getMiePhase(float cosTheta)
{
	const float g = 0.8;
	const float scale = 3.0 / (8.0 * PI);

	float num = (1.0 - g * g) * (1.0 + cosTheta * cosTheta);
	float denom = (2.0 + g * g) * pow((1.0 + g * g - 2.0 * g * cosTheta), 1.5);
	
	return scale * num/denom;
}

float getRayleighPhase(float cosTheta)
{
	const float k = 3.0 / (16.0 * PI);
	return k * (1.0 + cosTheta * cosTheta);
}

void getScatteringValues(vec3 pos, out vec3 rayleighScattering, out float mieScattering, out vec3 extinction)
{
	float altitudeKM = (length(pos) - groundRadius) * 1000.0;
	
	float rayleighDensity = exp(-altitudeKM / 8.0);
	float mieDensity = exp(-altitudeKM / 1.2);

	rayleighScattering = rayleighScatteringBase * rayleighDensity;
	float rayleighAborption = rayleighAbsorptionBase * rayleighDensity;

	mieScattering = meiScatteringBase * mieDensity;
	float mieAbsorption = mieAbsorptionBase * mieDensity;

	vec3 ozoneAbsorption = ozoneAbsorptionBase * max(0.0, 1.0 - abs(altitudeKM - 25.0) / 15.0);

	extinction = rayleighScattering + rayleighAborption + mieScattering + mieAbsorption + ozoneAbsorption;
}

float safeacos(const float x) 
{
    return acos(clamp(x, -1.0, 1.0));
}

float rayIntersectSphere(vec3 ro, vec3 rd, float rad) 
{
    float b = dot(ro, rd);
    float c = dot(ro, ro) - rad * rad;
    if (c > 0.0 && b > 0.0)
    {
        return -1.0;
    }
    float discr = b * b - c;
    if (discr < 0.0)
    {
        return -1.0;
    }
    // Special case: inside sphere, use far discriminant
    if (discr > b * b)
    {
        return (-b + sqrt(discr));
    }
    return -b - sqrt(discr);
}

vec3 getValFromTLUT(sampler2D tex, vec2 bufferRes, vec3 pos, vec3 sunDir) 
{
    float height = length(pos);
    vec3 up = pos / height;
	float sunCosZenithAngle = dot(sunDir, up);
    vec2 uv = vec2(textureSize(tex, 0).x * clamp(0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0),
                   textureSize(tex, 0).y * max(0.0, min(1.0, (height - groundRadius) / (atmosphereRadius - groundRadius))));
    uv /= bufferRes;
    return texture(tex, uv).rgb;
}

vec3 getValFromMultiScattLUT(sampler2D tex, vec2 bufferRes, vec3 pos, vec3 sunDir) 
{
    float height = length(pos);
    vec3 up = pos / height;
	float sunCosZenithAngle = dot(sunDir, up);
    vec2 uv = vec2(textureSize(tex, 0).x * clamp(0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0),
                   textureSize(tex, 0).y * max(0.0, min(1.0, (height - groundRadius) / (atmosphereRadius - groundRadius))));
    uv /= bufferRes;
    return texture(tex, uv).rgb;
}