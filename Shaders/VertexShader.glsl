#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec4 Normal;
out vec3 vertexPos;
out vec2 textCoord;

uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);
uniform mat4 proj = mat4(1);

void main()
{
    gl_Position = proj*view*model*vec4(position, 1.0);
    Normal = model*vec4(normal,1.0);
    vertexPos = vec3(model*vec4(position, 1.0));
    textCoord = uv;
}
