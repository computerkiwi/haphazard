#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;

out vec2 texCoord;
out vec4 texBounds;
flat out uint texLayer;

uniform uint TexLayer;
uniform vec4 TexBox;
uniform vec2 XRange;
uniform vec2 YRange;
uniform vec2 Percent;

void main()
{
	gl_Position = vec4(pos.xy, 0.0, 1.0);

	texCoord =  Percent + vec2(XRange.x + texcoord.x * XRange.y, YRange.x + texcoord.y * YRange.y);       //TexBox.xy + (texcoord * (TexBox.zw - TexBox.xy));
	texCoord.y = texcoord.y; // Disable vertical parallaxing

	texBounds = TexBox;
	texLayer = TexLayer;
}
