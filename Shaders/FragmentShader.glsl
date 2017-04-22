#version 410

in vec4 Normal;
in vec3 vertexPos;
in vec2 textCoord;

out vec4 outColor;

uniform vec4 color = vec4(1);
uniform vec3 cameraPos = vec3(0);
uniform int drawMode = 1;
uniform sampler2D tex;

void main()
{
  vec3 light = vec3(0, 10, 0);
  
  outColor = color;
  if(drawMode == 2)
    outColor = texture(tex, textCoord);  
  else if (drawMode == 1)
  {
    vec3 l = vec3(light-vertexPos);
  	l = normalize(l);
  	vec3 c = vec3(outColor);
  	vec3 n = vec3(Normal);
    n = normalize(n);
  	vec3 e = cameraPos-vertexPos;
    e = normalize(e);
  	vec3 h = normalize(e+l);

    vec3 lightColour = vec3(0.5);
    vec3 specularColour = vec3(0.75) + (0.25 * c);
    vec3 ambient = vec3(0.5);

    vec3 blinnPhongLighting = lightColour * specularColour * max(0, pow(dot(h, n), 2));
    vec3 diffuseReflection = c * (ambient + (lightColour * max(0, dot(n, l))));

    vec3 finalColour = diffuseReflection + blinnPhongLighting;

  	outColor = vec4(finalColour, 1);
  }
}
