#version 460 core

in vec3 tePos;
in vec3 teNormal;
in vec3 teColor;
in vec2 teUV;

out vec4 fragColor;

uniform vec3 lightDir;
uniform vec4 lightColor;
uniform vec3 viewPos;

uniform sampler2D albedoTex;
uniform float metallic = 0.0;
uniform float roughness = 0.35;
uniform float ao = 1.0;

uniform samplerCube irradianceMap;

#define PI 3.1415926538

float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 f0, float roughness);
vec3 acesTonemap(vec3 x);

void main()
{
	vec3 N = normalize(teNormal);
	vec3 V = normalize(viewPos - tePos);

	//vec3 albedo = pow(texture(albedoTex, vec2(teUV.x, -teUV.y)).rgb, vec3(2.2, 2.2, 2.2));
	vec3 albedo = texture(albedoTex, vec2(teUV.x, -teUV.y)).rgb;

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	vec3 lo = vec3(0.0);

	vec3 L = normalize(-lightDir);
	vec3 H = normalize(V + L);
	vec3 radiance = L * lightColor.rgb * lightColor.a;

	float NDF = distributionGGX(N, H, roughness);
	float G = geometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), f0, roughness);

	vec3 ks = F;
	vec3 kd = vec3(1.0) - ks;
	kd *= 1.0 - metallic;

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	float nDotL = max(dot(N, L), 0.0);
	lo += (kd * albedo / PI + specular) * radiance * nDotL;

	vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), f0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, vec3(N.x, -N.y, N.z)).rgb;
    vec3 diffuse = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
	//vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + lo;

	//color = acesTonemap(color);
	//color = color / (color + vec3(1.0));
	//color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 f0, float roughness)
{
	return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}