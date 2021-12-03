uniform vec2 lutRes;

const float PI = 3.14159265358;

const float groundRadius = 6360.0;
const float atmosphereRadius = 6460.0;

const vec3 groundAlbedo = vec3(0.3, 0.3, 0.3);

const vec3 rayleighScatteringBase = vec3(5802.0, 13558.0, 33100.0);
const float rayleighAbsorptionBase = 0.0;

const float meiScatteringBase = 3996.0;
const float mieAbsorptionBase = 4400.0;

const vec3 ozoneAbsorptionBase = vec3(650.0, 1881.0, 85.0);

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
	float altitudeKM = length(pos) - groundRadius;
	
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
    float c = dot(ro, ro) - rad*rad;
    if (c > 0.0f && b > 0.0) return -1.0;
    float discr = b*b - c;
    if (discr < 0.0) return -1.0;
    // Special case: inside sphere, use far discriminant
    if (discr > b*b) return (-b + sqrt(discr));
    return -b - sqrt(discr);
}

vec3 getValFromTLUT(sampler2D tex, vec2 bufferRes, vec3 pos, vec3 sunDir) 
{
    float height = length(pos);
    vec3 up = pos / height;
	float sunCosZenithAngle = dot(sunDir, up);
    vec2 uv = vec2(lutRes.x*clamp(0.5 + 0.5*sunCosZenithAngle, 0.0, 1.0),
                   lutRes.y*max(0.0, min(1.0, (height - groundRadius)/(atmosphereRadius - groundRadius))));
    uv /= bufferRes;
    return texture(tex, uv).rgb;
}

vec3 getValFromMultiScattLUT(sampler2D tex, vec2 bufferRes, vec3 pos, vec3 sunDir) 
{
    float height = length(pos);
    vec3 up = pos / height;
	float sunCosZenithAngle = dot(sunDir, up);
    vec2 uv = vec2(lutRes.x*clamp(0.5 + 0.5*sunCosZenithAngle, 0.0, 1.0),
                   lutRes.y*max(0.0, min(1.0, (height - groundRadius)/(atmosphereRadius - groundRadius))));
    uv /= bufferRes;
    return texture(tex, uv).rgb;
}