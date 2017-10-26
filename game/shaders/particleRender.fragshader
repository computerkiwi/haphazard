#version 330 core

in vec2 texcoord;
in float texLayer;
in vec4 color;

out vec4 outColor;

uniform sampler2DArray	tex;

void main()
{
	if(texLayer >= 0)
	{
		vec4 texColor = texture(tex, vec3(texcoord,texLayer));
		if(texColor.a < 0.1)
			discard;

		outColor = color * texColor;
	}
	else
		outColor = color;
}
