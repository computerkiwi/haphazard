#version 430 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 scale;
layout (location = 2) in float rotation;
layout (location = 3) in vec4 color;
layout (location = 4) in float shape;

#define SQUARE 0.0f
#define CIRCLE 1.0f

out VS_OUT {
	vec4 Offsets[4];
	vec2 Scale;
	vec4 Color;
	float Type;
} vs_out;

layout (std140, binding = 1) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

void main()
{	
	vs_out.Type = shape;

	if(shape == SQUARE)
	{
		float angle = rotation;

		mat4 s, t, r;
		s[0] = vec4( scale.x, 0, 0, 0); // Col 1
		s[1] = vec4( 0, scale.y, 0, 0); // Col 2
		s[2] = vec4( 0, 0, 1, 0); // Col 3
		s[3] = vec4( 0, 0, 0, 1); // Col 4

		
		t[0] = vec4(1,0,0,0); // Col 1
		t[1] = vec4(0,1,0,0); // Col 2
		t[2] = vec4(0,0,1,0);
		t[3] = vec4(pos.x,pos.y,0,1); // Col 4


		r[0] = vec4(cos(angle), sin(angle),0,0); // Col 1
		r[1] = vec4(-sin(angle), cos(angle),0,0); // Col 2
		r[2] = vec4(0,0,1,0);
		r[3] = vec4(0,0,0,1);
	
		mat4 model = t * r * s;


		mat4 PVM = proj * view * model;
		vs_out.Offsets[0] = PVM * vec4(-0.5, -0.5, 0.0, 1.0);
		vs_out.Offsets[1] = PVM * vec4(-0.5, 0.5, 0.0, 1.0);
		vs_out.Offsets[2] = PVM * vec4(0.5, 0.5, 0.0, 1.0);
		vs_out.Offsets[3] = PVM * vec4(0.5, -0.5, 0.0, 1.0);
	}
	else
	{
		vs_out.Scale.x = scale.x;
		vs_out.Scale.y = proj[1][1] / proj[0][0]; // Aspect ratio
		vs_out.Offsets[0] = proj * view * vec4(pos,0,1);
	}
	
	vs_out.Color = color;
}
