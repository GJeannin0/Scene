#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 TangentLightDir;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

const float ambientStrenght = 0.1;
const float specularStrength = 0.5;
const float specularPow = 52.0;
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
    //FragColor = texture(texture_diffuse, TexCoords);

    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);


    // Ambient
    vec3 ambient = ambientStrenght * lightColor;
    // Diffuse
    float diff = max(dot(TangentLightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDirection = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectionDirection = reflect(-TangentLightDir, normal);

    //vec3 halfwayDir = normalize(lightDirection + viewDirection);  
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPow);

    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), specularPow);
    vec3 specular = specularStrength * spec * lightColor;

    // Total light.
    vec3 resultDiffuseAmbient = 
        (ambient + diffuse) * texture(texture_diffuse, TexCoords).rgb; 
    vec3 resultSpecular =
        specular * texture(texture_specular, TexCoords).r;
    FragColor = vec4(resultDiffuseAmbient + resultSpecular, 1.0);
}