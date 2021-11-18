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

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor.rgb;

    //vec3 lightDir = normalize(lightPos - tePos);
    vec3 lightDirection = normalize(-lightDir);

    float diff = max(dot(teNormal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor.rgb;

    float specularStrength = 0.75;
    vec3 viewDir = normalize(viewPos - tePos);
    vec3 reflectDir = reflect(-lightDirection, teNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor.rgb;

    vec3 result = (ambient + diffuse + specular) * texture(albedoTex, vec2(teUV.x, -teUV.y)).rgb * teColor;

    fragColor = vec4(result, 1.0);
}