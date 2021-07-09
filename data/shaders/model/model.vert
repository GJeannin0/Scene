#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent; 

out vec3 outPosition;
out vec2 outTexCoords;
out vec3 outTangentCamera;
out vec3 outTangentPosition;
out vec3 outTangentLightPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 invModel;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

void main()
{
    outPosition = (model * vec4(aPos, 1.0)).xyz;
    outTexCoords = aTexCoords;


    mat3 normalMatrix = mat3(invModel);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T -dot(T,N)*N);
    vec3 B = normalize(cross(N,T));

    mat3 TBN = transpose(mat3(T,B,N));

    outTangentCamera = TBN * cameraPosition;
    outTangentPosition = TBN * outPosition;
    outTangentLightPosition = TBN * lightPosition;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}