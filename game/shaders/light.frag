#version 430 core

layout (std140, binding = 1) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

out vec2 Center;
in vec4 Color;
in float Range;
in vec2 Res;

out vec4 outColor;

void main()
{
	//float Dist = Center.x * gl_FragCoord.x + Center.y * gl_FragCoord.y;

	//if(Dist > Range)
	//	discard;

	outColor = vec4(1,0,1,1);
}
