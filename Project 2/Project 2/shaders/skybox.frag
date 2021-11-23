#version 460 core

in vec3 vsPos;

out vec4 fragColor;
  
uniform samplerCube environmentMap;
  
void main()
{
    vec3 pos = vec3(vsPos.x, -vsPos.y, vsPos.z);
    vec3 envColor = texture(environmentMap, pos).rgb;
    
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2));
  
    fragColor = vec4(envColor, 1.0);
}