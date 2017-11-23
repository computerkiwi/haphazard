#version 430 core

layout (location = 0) in vec2 vpos;

// instanced
layout (location = 1) in vec2 position;
layout (location = 2) in vec4 color;
layout (location = 3) in float range;

layout (std140, binding = 1) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

uniform vec2 Resolution;

out vec4 Color;
out vec4 SourcePos;
out float Range;
out vec2 Res;

void main()
{
	Color = color;
	Range = range ;

	SourcePos = proj * view * vec4(position, 0, 1.0); // World to screen

	gl_Position = vec4(vpos,0,1); //proj * view * vec4(vpos + position, 0, 1.0);

	Res = Resolution;
}
