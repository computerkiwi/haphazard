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
	float frame;
	float seed;
};

///
//  Uniform Buffer Objects
///

#define RENDER_UBO_SIZE 24
#define UPDATE_UBO_SIZE 43

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

ParticleSystem::ParticleSystem(const ParticleSystem& ps)
	: m_settings {ps.m_settings}
{
	Particle Particles[MAX_PARTICLES] = { 0 };

	Particles[0].type = static_cast<float>(EMITTER);
	Particles[0].position = glm::vec2(1, 0);
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

void ParticleSystem::Render(float dt, glm::vec2 pos, int id)
{
	glBindVertexArray(m_VAO);
	//glPointSize(10);
	UpdateParticles(dt, pos, id);
	RenderParticles(pos, id);

	std::swap(m_currVB, m_currTFB);
}

void ParticleSystem::UpdateParticles(float dt, glm::vec2 pos, int id)
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

	// UPDATE THE UPDATE_UBO_SIZE VARIABLE IF ADJUSTING THESE
	float data[] =
	{
		m_settings.burstEmission.x, m_settings.burstEmission.y, m_settings.burstEmission.z, 0,
		m_settings.scaleOverTime.x, m_settings.scaleOverTime.y, m_settings.scaleOverTime.z, m_settings.scaleOverTime.w,
		m_settings.startingVelocityVariance.x, m_settings.startingVelocityVariance.y,m_settings.startingVelocityVariance.z, m_settings.startingVelocityVariance.w,

		m_settings.emissionShapeScale.x, m_settings.emissionShapeScale.y,
		m_settings.startingVelocity.x, m_settings.startingVelocity.y,
		m_settings.acceleration.x, m_settings.acceleration.y,
		m_settings.trailScale.x, m_settings.trailScale.y,
		pos.x, pos.y,

		m_settings.particleLifetimeVariance.x,m_settings.particleLifetimeVariance.y,
		m_settings.startRotationVariation.x,m_settings.startRotationVariation.y,
		
		dt, 
		m_time + id * 1.2345f, // Some randomness added to time

		static_cast<float>(m_settings.isLooping),
		m_settings.emissionRate,
		static_cast<float>(m_settings.particlesPerEmission),
		static_cast<float>(m_settings.emissionShape),
		m_settings.emissionShapeScale.z, //EmissionShapeThickness
		1, //EmitAwayFromCenter
		
		m_settings.emitterLifetime,
		m_settings.particleLifetime,
		
		m_settings.startRotation,
		m_settings.rotationRate,
		
		static_cast<float>(m_settings.hasTrail),
		m_settings.trailEmissionRate,
		m_settings.trailLifetime,
		
		static_cast<float>(m_settings.particleSpace),

		0,
		0,
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


void ParticleSystem::RenderParticles(glm::vec2 pos, int id)
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
		0,0,1,1, // Default texture bounds
		pos.x, pos.y,
		static_cast<float>(m_settings.particleSpace),
		-1,		// Default texture (solid color square)
		1,  // Choose random color
	};

	if (m_settings.texture_resourceID != -1)
	{
		Texture *texture = static_cast<Texture *>(engine->GetResourceManager().Get(m_settings.texture_resourceID)->Data());
		data[23] = static_cast<float>(texture->GetLayer());

		data[16] = texture->GetBounds().x;
		data[17] = texture->GetBounds().y;
		data[18] = texture->GetBounds().z;
		data[19] = texture->GetBounds().w;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, renderSettingsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, RENDER_UBO_SIZE * sizeof(float), data);

	// Enable rendering
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

	//Particle p[3];
	//glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(Particle) * 3, &p);

	Shaders::particleRenderShader->ApplyAttributes();

	// Render it (with additive blending)
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);
	//glDrawArrays(GL_POINTS, 0, 3);

	// Reset Blend Mode
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
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


