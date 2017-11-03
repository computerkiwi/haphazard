#version 330 core

vec4 outColor;

/*
in vec3 Texcoord;
in vec4 Color;
*/

in vec4 texBox;
in float texLayer;

uniform sampler2DArray tex;

void main()
{
	outColor = vec4(1,1,1,1) + texBox + vec4(texLayer,0,0,0);
/*
	outColor = vec4(1,0,0,1);
	return;

	vec4 texColor = texture(tex, vec3(Texcoord));
	if(texColor.a < 0.1)
		discard;

	outColor = Color * texColor;
	*/
}