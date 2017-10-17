
/*
Some of the code below was influenced by the following resource:
http://ogldev.atspace.co.uk/www/tutorial28/tutorial28.html
*/
#include "Particles.h"
#include "Shaders.h"

#define MAX_PARTICLES 1000

///
// Enums
///

enum ParticleType
{
	EMITTER,
	PARTICLE,
	SUB_PARTICLE
};

///
// Structs
///

struct Particle
{
	float type;
	glm::vec2 position;
	glm::vec2 velocity;
	float life;
};

///
// ParticleSystem 
///

ParticleSystem::ParticleSystem(glm::vec2 position)
{
	Particle Particles[MAX_PARTICLES] = { 0 };

	Particles[0].type = static_cast<float>(EMITTER);
	Particles[0].position = position;
	Particles[0].velocity = glm::vec2(0.0f, 0.0001f);
	Particles[0].life = 0.0f;

	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);

	for (unsigned int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}
}

void ParticleSystem::Render(float dt)
{
	UpdateParticles(dt);
	RenderParticles();

	std::swap(m_currVB, m_currTFB);
}

void ParticleSystem::UpdateParticles(float dt)
{
	Shaders::particleUpdateShader->Use();

	// Don't want particle updates to be rendered to the screen, just sent through the feedback transform
	glEnable(GL_RASTERIZER_DISCARD);

	// Enable respective buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

	Shaders::particleUpdateShader->ApplyAttributes();

	// Make feedback transform active with topology of (GL_POINTS, GL_TRIANGLES, GL_LINES)
	glBeginTransformFeedback(GL_POINTS);

	// Check for first update time 
	if (m_isFirst) 
	{
		// Need to give the transform feedback loop a frame of reference for number of verts it will create
		glDrawArrays(GL_POINTS, 0, 1);
		m_isFirst = false;
	}
	else 
	{
		// Update particles (without drawing to screen)
		glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
	}
	
	glEndTransformFeedback(); // Done working with this TF
}



void ParticleSystem::RenderParticles()
{
	Shaders::particleRenderShader->Use();

	Shaders::particleRenderShader->SetVariable("")
	
	// Enable rendering
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

	Shaders::particleRenderShader->ApplyAttributes();

	// Render it
	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);
}
