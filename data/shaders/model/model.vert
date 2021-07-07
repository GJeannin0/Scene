#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 outPosition;
out vec3 outNormal;
out vec2 outTexCoords;
out vec3 outCamera;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 invModel;
uniform vec3 cameraPosition;

void main()
{
    outTexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    outPosition = (view * model * vec4(aPos, 1.0)).xyz;

    outNormal = vec3(invModel*vec4(aNormal,1.0));
    outCamera = cameraPosition;
}