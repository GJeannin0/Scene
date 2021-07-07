#version 300 es
precision highp float;

out vec4 FragColor;

in vec2 outTexCoords;
in vec3 outPosition;
in vec3 outNormal;
in vec3 outCamera;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform sampler2D NormalMap;


const float ambientStrenght = 0.1;
const float specularStrength = 0.5;
const float specularPow = 256.0;
const vec3 lightColor = vec3(1.0, 0.5, 0.5);
const vec3 lightPosition = vec3(0.0, 0.0, 3.0);


void main()
{
    vec3 ambient = ambientStrenght * lightColor;
    // Compute diffuse light.
    vec3 normalPlane = normalize(outNormal);

    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(NormalMap, outTexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0); 

    vec3 lightDirection = normalize(lightPosition - outPosition);
    float diff = max(dot(outNormal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Compute specular light.
    vec3 viewDirection = normalize(outCamera - outPosition);
    vec3 reflectionDirection = reflect(-lightDirection, outNormal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), specularPow);
    vec3 specular = specularStrength * spec * lightColor;

    // Total light.
    vec3 resultDiffuseAmbient = 
        (ambient + diffuse + specular) * texture(Diffuse, outTexCoords).rgb; 
    vec3 resultSpecular =
        specular * texture(Specular, outTexCoords).r;
    FragColor = vec4(resultDiffuseAmbient + resultSpecular, 1.0);
}