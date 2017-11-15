/*
FILE: Particles.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Particles.h"
#include "Shaders.h"
#include "Texture.h"
#include <cstdlib>
#include "Engine\Engine.h"
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
//  Uniform Buffer Objects
///

#define RENDER_UBO_SIZE 24
#define UPDATE_UBO_SIZE 35

static GLuint renderSettingsUBO = -1;
static GLuint updateSettingsUBO = -1;

static void GenerateUBOs()
{
	// Render
	glGenBuffers(1, &renderSettingsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, renderSettingsUBO);
	glBufferData(GL_UNIFORM_BUFFER, RENDER_UBO_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, renderSettingsUBO, 0, RENDER_UBO_SIZE * sizeof(float));

	GLuint renderBlockIndex = glGetUniformBlockIndex(Shaders::particleRenderShader->GetProgramID(), "Settings");
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, renderSettingsUBO); // Set this location to 2 (Camera matrices location = 1)
	glUniformBlockBinding(Shaders::particleRenderShader->GetProgramID(), renderBlockIndex, 2);

	// Update
	glGenBuffers(1, &updateSettingsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, updateSettingsUBO);
	glBufferData(GL_UNIFORM_BUFFER, UPDATE_UBO_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, updateSettingsUBO, 0, UPDATE_UBO_SIZE * sizeof(float));
	
	GLuint updateBlockIndex = glGetUniformBlockIndex(Shaders::particleUpdateShader->GetProgramID(), "UpdateSettings");
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, updateSettingsUBO); // Set this location to 3 (Camera matrices location = 1, render settings = 2)
	glUniformBlockBinding(Shaders::particleUpdateShader->GetProgramID(), updateBlockIndex, 3);
}

///
// ParticleSystem 
///

GLuint ParticleSystem::m_randTexture = -1;

ParticleSystem::ParticleSystem()
{
	if (m_randTexture == -1)
		GenRandomTexture();
	if (renderSettingsUBO == -1)
		GenerateUBOs();

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

	/*
	// Time
	Shaders::particleUpdateShader->SetVariable("dt", dt);
	Shaders::particleUpdateShader->SetVariable("Time", m_time);
	// Emission
	Shaders::particleUpdateShader->SetVariable("IsLooping", m_settings.isLooping);
	Shaders::particleUpdateShader->SetVariable("EmissionRate", m_settings.emissionRate);
	Shaders::particleUpdateShader->SetVariable("ParticlesPerEmission", m_settings.particlesPerEmission);
	Shaders::particleUpdateShader->SetVariable("BurstEmission", m_settings.burstEmission);
	Shaders::particleUpdateShader->SetVariable("EmissionShape", static_cast<int>(m_settings.emissionShape) );
	Shaders::particleUpdateShader->SetVariable("EmissionShapeScale", m_settings.emissionShapeScale);
	Shaders::particleUpdateShader->SetVariable("EmitterPosition", pos);
	// Lifetimes
	Shaders::particleUpdateShader->SetVariable("EmitterLifetime", m_settings.emitterLifetime);
	Shaders::particleUpdateShader->SetVariable("ParticleLifetime", m_settings.particleLifetime);
	Shaders::particleUpdateShader->SetVariable("ParticleLifetimeVariance", m_settings.particleLifetimeVariance);
	// Movement
	Shaders::particleUpdateShader->SetVariable("StartingVelocity", m_settings.startingVelocity);
	Shaders::particleUpdateShader->SetVariable("StartingVelocityVariance", m_settings.startingVelocityVariance);
	Shaders::particleUpdateShader->SetVariable("Acceleration", m_settings.acceleration);
	// Scale
	Shaders::particleUpdateShader->SetVariable("ScaleOverTime", m_settings.scaleOverTime);
	// Rotation
	Shaders::particleUpdateShader->SetVariable("StartRotation", m_settings.startRotation);
	Shaders::particleUpdateShader->SetVariable("StartRotationVariation", m_settings.startRotationVariation);
	Shaders::particleUpdateShader->SetVariable("RotationRate", m_settings.rotationRate);
	// Trail
	Shaders::particleUpdateShader->SetVariable("HasTrail", m_settings.hasTrail);
	Shaders::particleUpdateShader->SetVariable("TrailEmissionRate", m_settings.trailEmissionRate);
	Shaders::particleUpdateShader->SetVariable("TrailLifetime", m_settings.trailLifetime);
	Shaders::particleUpdateShader->SetVariable("TrailScale", m_settings.trailScale);
	*/

	float data[] =
	{
		dt, m_time,
		static_cast<float>(m_settings.isLooping),
		m_settings.emissionRate,
		static_cast<float>(m_settings.particlesPerEmission),
		m_settings.burstEmission.x, m_settings.burstEmission.y, m_settings.burstEmission.z,
		static_cast<float>(m_settings.emissionShape),
		m_settings.emissionShapeScale.x, m_settings.emissionShapeScale.y,
		m_settings.emitterLifetime,
		m_settings.particleLifetime,
		m_settings.particleLifetimeVariance,
		m_settings.startingVelocity.x, m_settings.startingVelocity.y,
		m_settings.startingVelocityVariance.x, m_settings.startingVelocityVariance.y,
		m_settings.acceleration.x, m_settings.acceleration.y,
		m_settings.scaleOverTime.x, m_settings.scaleOverTime.y, m_settings.scaleOverTime.z, m_settings.scaleOverTime.w,
		m_settings.startRotation,
		m_settings.startRotationVariation,
		m_settings.rotationRate,
		static_cast<float>(m_settings.hasTrail),
		m_settings.trailEmissionRate,
		m_settings.trailLifetime,
		m_settings.trailScale.x, m_settings.trailScale.y,
		static_cast<float>(m_settings.particleSpace),
		pos.x, pos.y
	};

	glBindBuffer(GL_UNIFORM_BUFFER, updateSettingsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, UPDATE_UBO_SIZE * sizeof(float), data);

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

	float data[] =
	{
		m_settings.startColor.x,		m_settings.startColor.y, m_settings.startColor.z, m_settings.startColor.w,
		m_settings.endColor.x,			m_settings.endColor.y, m_settings.endColor.z, m_settings.endColor.w,
		m_settings.trailStartColor.x,	m_settings.trailStartColor.y, m_settings.trailStartColor.z, m_settings.trailStartColor.w,
		m_settings.trailEndColor.x,		m_settings.trailEndColor.y, m_settings.trailEndColor.z, m_settings.trailEndColor.w,
		static_cast<float>(m_settings.particleSpace),
		pos.x, pos.y,
		-1,		// Default texture (solid color square)
		0,0,0,0 // Default texture bounds
	};

	if (false && m_settings.texture_resourceID != -1)
	{
		Texture *texture = static_cast<Texture *>(engine->GetResourceManager().Get(m_settings.texture_resourceID)->Data());
		data[19] = static_cast<float>(texture->GetID());

		data[22] = texture->GetBounds().x;
		data[23] = texture->GetBounds().y;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, renderSettingsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, RENDER_UBO_SIZE * sizeof(float), data);

	/*
	Shaders::particleRenderShader->SetVariable("StartColor", m_settings.startColor);
	Shaders::particleRenderShader->SetVariable("EndColor", m_settings.endColor);

	Shaders::particleRenderShader->SetVariable("TrailStartColor", m_settings.trailStartColor);
	Shaders::particleRenderShader->SetVariable("TrailEndColor", m_settings.trailEndColor);

	Shaders::particleRenderShader->SetVariable("SimulationSpace", static_cast<int>(m_settings.particleSpace) );
	Shaders::particleRenderShader->SetVariable("EmitterPosition", pos);

	if (m_settings.texture_resourceID != -1)
	{
		Texture *texture = static_cast<Texture *>(engine->GetResourceManager().Get(m_settings.texture_resourceID)->Data());
		Shaders::particleRenderShader->SetVariable("TextureLayer", static_cast<float>(texture->GetID()));
		Shaders::particleRenderShader->SetVariable("TextureBox", texture->GetBounds());
	}
	else
	{
		Shaders::particleRenderShader->SetVariable("TextureLayer", -1.0f);
	}
	*/

	// Enable rendering
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

	Particle p[3];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(Particle) * 3, &p);

	Shaders::particleRenderShader->ApplyAttributes();

	// Render it (with additive blending)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);
	//glDrawArrays(GL_POINTS, 0, 3);

	// Reset Blend Mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
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


