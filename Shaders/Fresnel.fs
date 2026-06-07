#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;
uniform vec3 fresnelColor;
uniform float fresnelPower;
uniform float fresnelIntensity;

void main()
{
    vec4 textura = texture(texture_diffuse1, TexCoord);

    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float fresnel = 1.0 - max(dot(viewDir, normal), 0.0);
    fresnel = pow(fresnel, fresnelPower);

    vec3 colorFinal = textura.rgb + fresnelColor * fresnel * fresnelIntensity;

    FragColor = vec4(colorFinal, textura.a);
}