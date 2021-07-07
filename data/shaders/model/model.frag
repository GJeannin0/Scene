#version 300 es
precision highp float;

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D Diffuse;

void main()
{
    FragColor = texture(Diffuse, TexCoords);
}