#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"

class ParticleSystem 
{
public:
	ParticleSystem(glm::vec2 position = glm::vec2(0,0));
	void Render(float dt);
private:
	void UpdateParticles(float dt);
	void RenderParticles();
	void GenRandomTexture();

	int m_isFirst = 0; // Is first time rendering
	unsigned int m_currVB = 0; // Current VBO index 
	unsigned int m_currTFB = 1; // Current Transform Feedback index

	float m_time = 0;

	// Buffers
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];

	static GLuint m_randTexture;
};

/*

------uniform float	dt;
------uniform float	EmitterLifetime;
------uniform float	ParticleLifetime;

------uniform bool	IsLooping;

------uniform vec2	StartingVelocity;

------uniform int		ParticlesPerEmission;
------uniform float	EmissionRate;
------uniform vec3	BurstEmission;			//(amt min, amt max, reoccurance rate)
------uniform int		EmissionShape;
------uniform vec2	EmissionShapeScale;

------uniform vec2 emitterPosition;


=====================
------uniform vec2	RotationOverLifetime;
------uniform vec4	ScaleOverLifetime;



#define EMITTER_TYPE 0.0f
#define PARTICLE_TYPE 1.0f

#define SPACE_WORLD 0
#define SPACE_LOCAL 1

#define SHAPE_CIRCLE_VOLUME 0
#define SHAPE_CIRLCE_EDGE   1
#define SHAPE_SQUARE_VOLUME 2
#define SHAPE_SQUARE_EDGE   3

*/