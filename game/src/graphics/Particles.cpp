
#include "Particles.h"
#include "Shaders.h"
#include <cstdlib>

#define MAX_PARTICLES 100

///
// Enums
///

enum ParticleType
{
	EMITTER,
	PARTICLE
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

GLuint ParticleSystem::m_randTexture = -1;

ParticleSystem::ParticleSystem(glm::vec2 position)
{
	if (m_randTexture == -1)
		GenRandomTexture();

	Particle Particles[MAX_PARTICLES] = { 0 };

	Particles[0].type = static_cast<float>(EMITTER);
	Particles[0].position = glm::vec2(1,0);
	Particles[0].velocity = glm::vec2(0.0f, 1.0f);
	Particles[0].life = 0.0f;

	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);

	for (int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}


	Shaders::particleUpdateShader->Use();
	glBindTexture(GL_TEXTURE_1D, m_randTexture);
}


void ParticleSystem::Render(float dt)
{
	glPointSize(10);
	UpdateParticles(dt);
	RenderParticles();

	std::swap(m_currVB, m_currTFB);
}

void ParticleSystem::UpdateParticles(float dt)
{
	m_time += dt;

	Shaders::particleUpdateShader->Use();

	// Don't want particle updates to be rendered to the screen, just sent through the feedback transform
	glEnable(GL_RASTERIZER_DISCARD);

	// Enable buffer input and output
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

	Shaders::particleUpdateShader->ApplyAttributes();

	// Set state
	Shaders::particleUpdateShader->SetVariable("dt", dt);
	Shaders::particleUpdateShader->SetVariable("Time", m_time);
	Shaders::particleUpdateShader->SetVariable("EmitterLifetime", 5.0f);
	Shaders::particleUpdateShader->SetVariable("ParticleLifetime", 1.5f);
	Shaders::particleUpdateShader->SetVariable("ParticleLifetimeVariance", 0.2f);
	Shaders::particleUpdateShader->SetVariable("IsLooping", true);
	Shaders::particleUpdateShader->SetVariable("StartingVelocity", glm::vec2(0,2));
	Shaders::particleUpdateShader->SetVariable("StartingVelocityVariance", glm::vec2(0.5f, 0.3f));
	Shaders::particleUpdateShader->SetVariable("Acceleration", glm::vec2(0, -2));
	Shaders::particleUpdateShader->SetVariable("ParticlesPerEmission", 1);
	Shaders::particleUpdateShader->SetVariable("EmissionRate", 0.1f);
	Shaders::particleUpdateShader->SetVariable("BurstEmission", glm::vec3(10,10,1));
	Shaders::particleUpdateShader->SetVariable("EmissionShape", 0);
	Shaders::particleUpdateShader->SetVariable("EmissionShapeScale", glm::vec2(0.5f,0.5f) );
	Shaders::particleUpdateShader->SetVariable("EmitterPosition", glm::vec2(-1,0));

	// Make feedback transform active with topology of (GL_POINTS, GL_TRIANGLES, GL_LINES)
	glBeginTransformFeedback(GL_POINTS);

	// Check for first update time 
	if (m_isFirst < 1) 
	{
		// Need to give the transform feedback loop a frame of reference for number of verts it will create
		glDrawArrays(GL_POINTS, 0, 1);
		m_isFirst++;
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

	// Set state, if needed (texture, ect)
	Shaders::particleRenderShader->SetVariable("ParticleLife", 1.5f);
  Shaders::particleRenderShader->SetVariable("StartColor", glm::vec4(1, 0, 0, 1));
	Shaders::particleRenderShader->SetVariable("EndColor", glm::vec4(0, 1, 0, 1));
	Shaders::particleRenderShader->SetVariable("SimulationSpace", 0);
	Shaders::particleRenderShader->SetVariable("EmitterPosition", glm::vec2(-1,0));
	Shaders::particleRenderShader->SetVariable("RotationOverLifetime", glm::vec2(0, 3.1415926));
	Shaders::particleRenderShader->SetVariable("ScaleOverLifetime", glm::vec4(0.1f,0.1f, 0.01f,0.01f));

	// Enable rendering
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

	Particle p[3];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(Particle) * 3, &p);

	Shaders::particleRenderShader->ApplyAttributes();

	// Render it
	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);
	//glDrawArrays(GL_POINTS, 0, 3);
}

void ParticleSystem::GenRandomTexture()
{
	const int size = 1000;
	glm::vec3 data[size];

	for (unsigned int i = 0; i < size; i++) {
		data[i].x = static_cast<float>(rand()) / RAND_MAX;
		data[i].y = static_cast<float>(rand()) / RAND_MAX;
		data[i].z = static_cast<float>(rand()) / RAND_MAX;
	}

	glGenTextures(1, &m_randTexture);
	glBindTexture(GL_TEXTURE_1D, m_randTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0.0f, GL_RGB, GL_FLOAT, data);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
}
