#version 300 es
precision highp float;

out vec4 FragColor;

in vec2 outTexCoords;
in vec3 outPosition;
in vec3 outTangentCamera;
in vec3 outTangentPosition;
in vec3 outTangentLightPosition;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;


const float ambientStrenght = 0.1;
const float specularStrength = 0.5;
const float specularPow = 32.0;
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
//const vec3 lightPosition = vec3(0.0, 0.0, 3.0);


void main()
{
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal, outTexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);


    // Ambient
    vec3 ambient = ambientStrenght * lightColor;
    // Diffuse
    vec3 lightDirection = normalize(outTangentLightPosition - outTangentPosition);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDirection = normalize(outTangentCamera - outTangentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normal);

    //vec3 halfwayDir = normalize(lightDirection + viewDirection);  
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPow);

    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), specularPow);
    vec3 specular = specularStrength * spec * lightColor;

    // Total light.
    vec3 resultDiffuseAmbient = 
        (ambient + diffuse) * texture(texture_diffuse, outTexCoords).rgb; 
    vec3 resultSpecular =
        specular * texture(texture_specular, outTexCoords).r;
    FragColor = vec4(resultDiffuseAmbient + resultSpecular, 1.0);
    //FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(normal,1.0);
}