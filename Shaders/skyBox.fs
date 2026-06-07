#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skyboxDia;
uniform samplerCube skyboxNoche;
uniform float factorDiaNoche;

void main()
{
    vec4 colorDia = texture(skyboxDia, TexCoords);
    vec4 colorNoche = texture(skyboxNoche, TexCoords);

    FragColor = mix(colorDia, colorNoche, factorDiaNoche);
}