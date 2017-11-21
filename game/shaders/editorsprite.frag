#version 430 core

in vec3 Texcoord;

out vec4 FragColor;

uniform sampler2DArray tex;

void main()
{
	vec4 texColor = texture(tex, Texcoord);

	if(texColor.a < 0.1)
		discard;

	FragColor = texColor;
}
