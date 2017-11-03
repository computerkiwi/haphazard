#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;

out vec2 texCoord;
flat out uint texLayer;

uniform uint TexLayer;
uniform vec4 TexBox;

void main()
{
	gl_Position = vec4(pos.xy, 0.0, 1.0);

	texCoord = TexBox.xy + (texcoord * (TexBox.zw - TexBox.xy));
	texLayer = TexLayer;
}
