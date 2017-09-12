#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D brights;

void main()
{
    vec3 sceneColor = texture(screenTexture, TexCoord).rgb;
    vec3 bloomColor = texture(brights, TexCoord).rgb;
    sceneColor += bloomColor; // additive blending


    FragColor = vec4(sceneColor, 1.0);
}