#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "Texture.h"

enum EmissionShape
{
	POINT		  = 0,
	CIRCLE_VOLUME = 1,
	CIRLCE_EDGE	  = 2,
	SQUARE_VOLUME = 3
};

enum SimulationSpace
{
	WORLD = 0,
	LOCAL = 1
};

struct ParticleSettings
{
	// Emission
	bool			isLooping = true;				 // Emitter loops or dies after lifetime
	float			EmissionRate = 1;				 // Time (in seconds) between each particle spawning
	int				ParticlesPerEmission = 1;		 // Particles emitted per emission
	glm::vec3		BurstEmission = {0,0,0};		 // Burst settings: particles min, particles max, reoccurance rate in seconds
	EmissionShape	EmissionShape = POINT;			 // Shape particles are emitted in
	glm::vec2		EmissionShapeScale = {1,1};		 // Scale of emission shape around center point of emission
	SimulationSpace ParticleSpace = WORLD;			 // Particle simulation space
	// Lifetimes									 
	float			EmitterLifetime = 1;			 // Lifetime of emitter (applicable only if isLooping = false)
	float			ParticleLifetime = 1;			 // Lifetime of particle
	float			ParticleLifetimeVariance = 0;	 // Variation of life of particle in seconds, between -Variation/2 and +Variation/2
	// Movement										 
	glm::vec2		StartingVelocity = {0,0};		 // Velocity of particle at creation in world units per second
	glm::vec2		StartingVelocityVariance = {0,0};// Variation of starting velocity in each direction, between -Variation/2 and +Variation/2
	glm::vec2		Acceleration = {0,0};			 // Acceleration of particle in world units per second per second
	// Scale										 
	glm::vec4		ScaleOverTime = {1,1,1,1};		 // Start scale, end scale. Particles spawned at start scale and linearly interpolate to end scale over their lifetime
	// Rotation										 
	float			StartRotation = 0;				 // Start rotation of particle in radians
	float			StartRotationVariation = 0;		 // Variation of starting rotation of particle in seconds, between -Variation/2 and +Variation/2
	float			RotationRate = 0;				 // Rotation in radians per second
	// Render
	glm::vec4		StartColor = {1,1,1,1};			 // Blend color of particle at start of life
	glm::vec4		EndColor = {1,1,1,1};			 // Blend color of particle at end of life, linearly interpolated from start color through lifetime
	Texture*		Texture = nullptr;				 // Texture of particle
	// Trail										 
	bool			HasTrail = false;				 // Spawn trail particles
	float			TrailEmissionRate = 0.05f;		 // Emission rate of trail particles, lower for smoother trail
	float			TrailLifetime = 1;				 // Lifetime of trail particles in seconds
	glm::vec2		TrailScale = {1,1};				 // Scale of trail particles RELATIVE TO SOURCE PARTICLE
	glm::vec4		TrailStartColor = {1,1,1,1};	 // Color of trail near particle
	glm::vec4		TrailEndColor = {1,1,1,1};		 // Color of trail near tail
};

class ParticleSystem 
{
public:
	ParticleSystem(glm::vec2 position = glm::vec2(0,0));
	void Render(float dt, glm::vec2 pos);


	// Emitter loops or dies after lifetime
	void SetIsLooping(bool loop) { m_settings.isLooping = true; }

	// Time (in seconds) between each particle spawning
	void SetEmissionRate(float rate) { m_settings.EmissionRate = rate; }

	// Particles emitted per emission
	void SetParticlesPerEmission(int amt) { m_settings.ParticlesPerEmission = amt; }
	
	// Set Burst minimum and maximum amount of particles spawned in each burst (Also set Burst Emission Rate!)
	void SetBurstEmissionParticles(int minParticles, int maxParticles) { m_settings.BurstEmission.x = minParticles; m_settings.BurstEmission.y = maxParticles; }
	
	// Set reoccurance rate for burst particles in seconds (Also set Burst Emission Particles!)
	void SetBurstEmissionRate(float repeatRate) { m_settings.BurstEmission.z = repeatRate; }

	// Shape particles are emitted in and the scale of that shape, scaled around the center
	void SetEmissionShape(EmissionShape shape, float scaleX, float scaleY) { m_settings.EmissionShape = shape; m_settings.EmissionShapeScale = glm::vec2(scaleX, scaleY); }
	
	// Simulation space of particles. Local moves all particles relative to emitter, world move particles independantly of emitter
	void SetSimulationSpace(SimulationSpace space) { m_settings.ParticleSpace = space; }

	// Lifetime of emitter (only applicable if isLooping = false, otherwise it does nothing)
	void SetEmitterLifetime(float life) { m_settings.EmitterLifetime = life; }
	
	// Lifetime of particle in seconds, and variation of life of particle, between -Variation/2 and +Variation/2
	void SetParticleLifetime(float life, float variance = 0) { m_settings.ParticleLifetime = life; m_settings.ParticleLifetimeVariance = variance; }
	
	// Velocity of particle at creation in world units per second, and variation of starting velocity in each direction, between -Variation/2 and +Variation/2
	void SetVelocity(glm::vec2 vel, glm::vec2 variance = glm::vec2(0, 0)) { m_settings.StartingVelocity = vel; m_settings.StartingVelocityVariance = variance; }
	
	// Acceleration of particle in world units per second per second
	void SetAcceleration(glm::vec2 a) { m_settings.Acceleration = a; }
	
	// Particles spawned at start scale and linearly scale to end scale over their lifetime
	void SetScaleOverLife(glm::vec2 startScale, glm::vec2 endScale) { m_settings.ScaleOverTime = glm::vec4(startScale, endScale); }
	
	// Start rotation of particle, and variation of start rotation between -Variation/2 and +Variation/2
	void SetStartRotation(float degrees, float variation = 0) { m_settings.StartRotation = degrees * 3.14159265359f / 180.0f; m_settings.StartRotationVariation = variation * 3.14159265359f / 180.0f; }
	
	// Rotation in degrees per second
	void SetRotationRate(float rate) { m_settings.RotationRate = rate; }
	
	// Blend color of particle over lifetime linearly interpolated from start to end over the particle's lifetime
	void SetColor(glm::vec4 start, glm::vec4 end) { m_settings.StartColor = start; m_settings.EndColor = end; }

	// Set's particle texture. Does not apply to trail particles. Particle color will be blended with texture
	void SetTexture(Texture* texture) { m_settings.Texture = texture; }

	// True to spawn trail particles
	void SetHasTrail(bool trail) { m_settings.HasTrail = trail; }
	
	// Emission rate of trail particles, lower for smoother trail (Beware not to spawn too many or else max particle limit will be reached quickly)
	void SetTrailEmissionRate(float rate) { m_settings.TrailEmissionRate = rate; }
	
	// Lifetime of trail particles in seconds
	void SetTrailLifetime(float life) { m_settings.TrailLifetime = life; }
	
	// Scale of trail particles RELATIVE TO SOURCE PARTICLE (use (1,1) to match scale of particle it trails)
	void SetTrailScale(glm::vec2 scale) { m_settings.TrailScale = scale; }

	// Set color of trail, start color is color of trail near particle, end color is color of trail near tail end
	void SetTrailColor(glm::vec4 start, glm::vec4 end) { m_settings.TrailStartColor = start; m_settings.TrailEndColor = end; }

private:
	void UpdateParticles(float dt, glm::vec2 pos);
	void RenderParticles(glm::vec2 pos);
	void GenRandomTexture();

	int m_isFirst = 0; // Is first time rendering
	unsigned int m_currVB = 0; // Current VBO index 
	unsigned int m_currTFB = 1; // Current Transform Feedback index

	float m_time = 0; // Time particle has been alive, used for random generation seed in shader
	ParticleSettings m_settings; // Particle settings (So much memory!)

	// Buffers
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
	GLuint m_VAO;

	static GLuint m_randTexture;
};
