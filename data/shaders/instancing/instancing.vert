#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent; 
layout (location = 4) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 TangentLightDir;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 cameraMatrix;
uniform vec3 viewPos;

const vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));

void main()
{

    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));   
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(aInstanceMatrix)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    TangentLightDir = TBN * lightDir;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;
        
    gl_Position = cameraMatrix * aInstanceMatrix * vec4(aPos, 1.0);
}