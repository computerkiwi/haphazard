#version 330 core

in vec2 texCoord;
flat in uint texLayer;

out vec4 FragColor;

uniform sampler2DArray tex;

void main()
{
	vec4 texColor = texture(tex, vec3(texCoord,texLayer));

	if(texColor.a < 0.1)
		discard;

	FragColor = texColor;
}