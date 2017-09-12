#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;

out vec2 TexCoord;
out vec2 Offset;
out float Alpha;
out float Depth;

uniform vec3 position;
uniform vec3 scale;
uniform float alpha;

void main()
{
	vec2 off = pos.xy * scale.xy + position.xy;

	gl_Position = vec4(off.xy, 0.0, 1.0);
	TexCoord = texcoord;
	Offset = off;
	Alpha = alpha;
	Depth = scale.z;
}
