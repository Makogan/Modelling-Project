#version 410

in vec4 Normal;
in vec3 vertexPos;

out vec4 outColor;

uniform vec4 color = vec4(1);
uniform vec3 cameraPos;

uniform vec3 lightDirection = vec3(-1,0,0);
uniform vec3 lightColor = vec3(1,1,1);
uniform float ambientIntensity = 0.6f;

void main()
{
    float diffuseIntensity = max(0.0f, dot(normalize(vec3(Normal)), -lightDirection));
    outColor = color*vec4(lightColor*(ambientIntensity + diffuseIntensity), 1.0f);
}