/*
FILE: Particles.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Particles.h"
#include "Shaders.h"
#include "Texture.h"
#include <cstdlib>

#define MAX_PARTICLES 500

///
// Enums
///

enum ParticleType
{
	EMITTER,
	PARTICLE,
	TRAIL_PARTICLE,
};

///
// Structs
///

struct Particle
{
	float type;
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 scale;
	float rotation;
	float life;
	float maxLife;
};

///
// ParticleSystem 
///

GLuint ParticleSystem::m_randTexture = -1;

ParticleSystem::ParticleSystem()
{
	if (m_randTexture == -1)
		GenRandomTexture();

	Particle Particles[MAX_PARTICLES] = { 0 };

	Particles[0].type = static_cast<float>(EMITTER);
	Particles[0].position = glm::vec2(1,0);
	Particles[0].velocity = glm::vec2(0.0f, 1.0f);
	Particles[0].scale = glm::vec2(1.0f, 1.0f);
	Particles[0].life = 0.0f;

	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);
	glGenVertexArrays(1, &m_VAO);

	for (int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}


	Shaders::particleUpdateShader->Use();
	glBindTexture(GL_TEXTURE_1D, m_randTexture);
}


void ParticleSystem::Render(float dt, glm::vec2 pos)
{
	glBindVertexArray(m_VAO);
	//glPointSize(10);
	UpdateParticles(dt, pos);
	RenderParticles(pos);

	std::swap(m_currVB, m_currTFB);
}

void ParticleSystem::UpdateParticles(float dt, glm::vec2 pos)
{
	m_time += dt;

	Shaders::particleUpdateShader->Use();

	// Don't want particle updates to be rendered to the screen, just sent through the feedback transform
	glEnable(GL_RASTERIZER_DISCARD);

	// Enable buffer input and output
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

	Shaders::particleUpdateShader->ApplyAttributes();
	
	///
	// Set settings
	///

	// Time
	Shaders::particleUpdateShader->SetVariable("dt", dt);
	Shaders::particleUpdateShader->SetVariable("Time", m_time);
	// Emission
	Shaders::particleUpdateShader->SetVariable("IsLooping", m_settings.isLooping);
	Shaders::particleUpdateShader->SetVariable("EmissionRate", m_settings.EmissionRate);
	Shaders::particleUpdateShader->SetVariable("ParticlesPerEmission", m_settings.ParticlesPerEmission);
	Shaders::particleUpdateShader->SetVariable("BurstEmission", m_settings.BurstEmission);
	Shaders::particleUpdateShader->SetVariable("EmissionShape", static_cast<int>(m_settings.EmissionShape) );
	Shaders::particleUpdateShader->SetVariable("EmissionShapeScale", m_settings.EmissionShapeScale);
	Shaders::particleRenderShader->SetVariable("SimulationSpace", static_cast<int>(m_settings.ParticleSpace));
	Shaders::particleUpdateShader->SetVariable("EmitterPosition", pos);
	// Lifetimes
	Shaders::particleUpdateShader->SetVariable("EmitterLifetime", m_settings.EmitterLifetime);
	Shaders::particleUpdateShader->SetVariable("ParticleLifetime", m_settings.ParticleLifetime);
	Shaders::particleUpdateShader->SetVariable("ParticleLifetimeVariance", m_settings.ParticleLifetimeVariance);
	// Movement
	Shaders::particleUpdateShader->SetVariable("StartingVelocity", m_settings.StartingVelocity);
	Shaders::particleUpdateShader->SetVariable("StartingVelocityVariance", m_settings.StartingVelocityVariance);
	Shaders::particleUpdateShader->SetVariable("Acceleration", m_settings.Acceleration);
	// Scale
	Shaders::particleUpdateShader->SetVariable("ScaleOverTime", m_settings.ScaleOverTime);
	// Rotation
	Shaders::particleUpdateShader->SetVariable("StartRotation", m_settings.StartRotation);
	Shaders::particleUpdateShader->SetVariable("StartRotationVariation", m_settings.StartRotationVariation);
	Shaders::particleUpdateShader->SetVariable("RotationRate", m_settings.RotationRate);
	// Trail
	Shaders::particleUpdateShader->SetVariable("HasTrail", m_settings.HasTrail);
	Shaders::particleUpdateShader->SetVariable("TrailEmissionRate", m_settings.TrailEmissionRate);
	Shaders::particleUpdateShader->SetVariable("TrailLifetime", m_settings.TrailLifetime);
	Shaders::particleUpdateShader->SetVariable("TrailScale", m_settings.TrailScale);

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


void ParticleSystem::RenderParticles(glm::vec2 pos)
{
	Shaders::particleRenderShader->Use();

	Texture::BindArray();

	// Set rendering state (color, texture, etc)
	Shaders::particleRenderShader->SetVariable("StartColor", m_settings.StartColor);
	Shaders::particleRenderShader->SetVariable("EndColor", m_settings.EndColor);

	Shaders::particleRenderShader->SetVariable("TrailStartColor", m_settings.TrailStartColor);
	Shaders::particleRenderShader->SetVariable("TrailEndColor", m_settings.TrailEndColor);

	Shaders::particleRenderShader->SetVariable("SimulationSpace", static_cast<int>(m_settings.ParticleSpace) );
	Shaders::particleRenderShader->SetVariable("EmitterPosition", pos);
	if (m_settings.Texture)
	{
		Shaders::particleRenderShader->SetVariable("TextureLayer", m_settings.Texture->GetID());
		Shaders::particleRenderShader->SetVariable("TextureBox", glm::vec4(0,0,m_settings.Texture->GetBounds()));
	}
	else
	{
		Shaders::particleRenderShader->SetVariable("TextureLayer", -1.0f);
	}

	// Enable rendering
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

	//Particle p[3];
	//glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(Particle) * 3, &p);

	Shaders::particleRenderShader->ApplyAttributes();

	// Render it (without blending, this will be done later by additively rendering the particle layer onto the screen)
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
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, data);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
}
