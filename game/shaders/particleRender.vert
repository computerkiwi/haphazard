#version 430 core

layout (location = 0) in float type;
layout (location = 1) in vec2 pos;
layout (location = 2) in vec2 vel;
layout (location = 3) in vec2 scale;
layout (location = 4) in float rotation;
layout (location = 5) in float life;
layout (location = 6) in float maxLife;

out float Type;
out vec2 Velocity;
out vec2 Texcoord;
out vec4 Color;
out float Rotation;
out vec2 Scale;
out vec2 IResolution;
out float TexLayer;
out vec4 TexBox;
out float PercentLife;


layout (std140, binding = 1) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

/// Enum Defines \\\

#define TYPE_EMITTER 0.0f
#define TYPE_PARTICLE 1.0f
#define TYPE_TRAIL 2.0f

#define SPACE_WORLD 0
#define SPACE_LOCAL 1

/// Uniform Settings \\\

layout (std140) uniform Settings
{
	vec4	StartColor;
	vec4	EndColor;

	vec4	TrailStartColor;
	vec4	TrailEndColor;

	float	SimulationSpace;
	vec2	EmitterPosition;

	float	TextureLayer;
	vec4	TextureBox;
};


void main()
{
	Type = type;
	Velocity = vel;

	TexLayer = TextureLayer;
	TexBox = TextureBox;


	if(SimulationSpace == SPACE_LOCAL)
		gl_Position = proj * view * vec4(pos, 0.0f, 1.0f);
	else
		gl_Position = proj * view * vec4(pos, 0.0f, 1.0f);

	float percent = life / maxLife;
	PercentLife = percent;
	

	if(type == TYPE_TRAIL)
	{
		TexLayer = -1;
		// MaxLife for trail particles is the parent's life percent
		// Because it is used to make a gradient over the whole trail rather than
		// per particle
		Color = TrailStartColor * (1 - maxLife) + TrailEndColor * maxLife;
	}
	else
		Color = StartColor * (1 - percent) + EndColor * percent;

	Rotation = rotation;
	Scale = scale;
	IResolution = vec2(proj[0][0], proj[1][1]);
}
