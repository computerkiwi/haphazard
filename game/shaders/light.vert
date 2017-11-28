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
out vec2 Center;
out float Range;
out vec2 Res;

void main()
{
	Color = color;
	Range = range;
	Res = Resolution;

	mat4 model;
	model[0] = model[1] = model[2] = vec4(0);
	model[0][0] = model[1][1] = model[2][2] = 1;
	model[3] = vec4(position.x, position.y, 0, 1);

	gl_Position = proj * view * model * vec4(vpos, 0, 1.0);

	Center = (proj * view * model * vec4(0, 0, 0, 1.0)).xy;

	//SourcePos = proj * view * vec4(position, 0, 1.0); // World to screen
	//SourcePos.x += 0.5;
	//SourcePos.y += 0.5 / (Resolution.x / Resolution.y);
}
