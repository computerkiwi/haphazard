/*
FILE: Particles.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

/*
Note to future self: Possible future optimizations
-Instead of using SetVariable, do it manually to save time with uniform setting
-In pixel renderer geo shader, texVerts can be optimized to not do calculations (may already be optimized out by compiler)
-Blendmode
*/
#pragma once

#include "Universal.h"
#include "GL\glew.h"
#include "glm\glm.hpp"
#include "Texture.h"

#include "Engine\ResourceManager.h"


#define MAX_PARTICLES 500
#define LARGE_MAX_PARTICLES 1500


// Editor Forward Declares
   class GameObject;
   class Editor;
// -----------------------

enum EmissionShape : int
{
	SHAPE_POINT		     = 0,
	SHAPE_CIRCLE_VOLUME  = 1,
	SHAPE_CIRLCE_EDGE	 = 2,
	SHAPE_SQUARE_VOLUME  = 3
};

enum SimulationSpace
{
	WORLD = 0,
	LOCAL = 1
};

struct ParticleSettings
{
	// Emission
	bool            increasedMaxParticles = false;        // Maximum particles at one time in this system
	bool            isLooping = true;            // Emitter loops or dies after lifetime
	float           emissionRate = 1;            // Time (in seconds) between each particle spawning
	int             particlesPerEmission = 1;    // Particles emitted per emission
	bool              emitBurstAtStart = true;
	glm::vec3	      burstEmission = {0.0f,0,0};  // Burst settings: particles min, particles max, reoccurance rate in seconds
	EmissionShape	  emissionShape = SHAPE_POINT; // Shape particles are emitted in
	glm::vec3	      emissionShapeScale = {1,1,0};	 // Scale of emission shape around center point of emission
	SimulationSpace particleSpace = WORLD;       // Particle simulation space
	float           emitOverDistanceAmount = 0;
	bool            emitAwayFromCenter = false;
	// Lifetimes
	float           emitterLifetime = 10;             // Lifetime of emitter (applicable only if isLooping = false)
	float           particleLifetime = 1;             // Lifetime of particle
	glm::vec2	      particleLifetimeVariance = {0,0}; // Variation of life of particle in seconds, between -Variation/2 and +Variation/2
	// Movement										 
	glm::vec2       startingVelocity = {0,0};             // Velocity of particle at creation in world units per second
	glm::vec4       startingVelocityVariance = {0,0,0,0}; // Variation of starting velocity in each direction, between -Variation/2 and +Variation/2
	glm::vec2       acceleration = {0,0};                 // Acceleration of particle in world units per second per second
	float           velocityLimitAmount = 1;
	// Scale										 
	glm::vec4       scaleOverTime = {0.1f,0.1f,0.1f,0.1f}; // Start scale, end scale. Particles spawned at start scale and linearly interpolate to end scale over their lifetime
	// Rotation										 
	float           startRotation = 0;              // Start rotation of particle in radians
	glm::vec2       startRotationVariation = {0,0}; // Variation of starting rotation of particle in seconds, between -Variation/2 and +Variation/2
	float           rotationRate = 0;               // Rotation over lifetime in radians
	float           speedScaledRotation = 0;
	// Render
	bool			randomBetweenColors;     // Use a random between start/end instead of being a gradient
	glm::vec4       startColor = {1,1,1,1};  // Blend color of particle at start of life
	glm::vec4       endColor = {1,1,1,1};    // Blend color of particle at end of life, linearly interpolated from start color through lifetime
	ResourceID      texture_resourceID = -1; // Texture of particle
	// Trail										 
	bool            hasTrail = false;            // Spawn trail particles
	float           trailEmissionRate = 0.05f;   // Emission rate of trail particles, lower for smoother trail
	float	          trailLifetime = 1;           // Lifetime of trail particles in seconds
	glm::vec2       trailScale = {1,1};          // Scale of trail particles RELATIVE TO SOURCE PARTICLE
	glm::vec4       trailEndColor = {1,1,1,1};   // Color of trail near tail

private:
	// For meta.
	int GetEmissionShape() const { return emissionShape; }
	void SetEmissionShape(int shape) { emissionShape = static_cast<EmissionShape>(shape); }
	int GetParticleSpace() const { return particleSpace; }
	void SetParticleSpace(int space) { particleSpace = static_cast<SimulationSpace>(space); }

	META_REGISTER(ParticleSettings)
	{
		META_DefineType(EmissionShape);
		META_DefineType(SimulationSpace);

		META_DefineMember(ParticleSettings, increasedMaxParticles, "IncreasedMaxParticles");
		META_DefineMember(ParticleSettings, isLooping, "isLooping");

		META_DefineMember(ParticleSettings, emissionRate, "EmissionRate");
		META_DefineMember(ParticleSettings, particlesPerEmission, "ParticlesPerEmission");
		META_DefineMember(ParticleSettings, emitBurstAtStart, "EmitBurstAtStart");
		META_DefineMember(ParticleSettings, burstEmission, "BurstEmission");
		META_DefineGetterSetter(ParticleSettings, int, GetEmissionShape, SetEmissionShape, "EmissionShape");
		META_DefineMember(ParticleSettings, emissionShapeScale, "EmissionShapeScaleThickness");
		META_DefineGetterSetter(ParticleSettings, int, GetParticleSpace, SetParticleSpace, "ParticleSpace");
		META_DefineMember(ParticleSettings, emitOverDistanceAmount, "EmitOverDistanceAmount");
		META_DefineMember(ParticleSettings, emitAwayFromCenter, "EmitAwayFromCenter");

		META_DefineMember(ParticleSettings, emitterLifetime, "EmitterLifetime");
		META_DefineMember(ParticleSettings, particleLifetime, "ParticleLifetime");
		META_DefineMember(ParticleSettings, particleLifetimeVariance, "ParticleLifetimeMinMax");

		META_DefineMember(ParticleSettings, startingVelocity, "StartingVelocity");
		META_DefineMember(ParticleSettings, startingVelocityVariance, "StartingVelocityMinMax");
		META_DefineMember(ParticleSettings, acceleration, "Acceleration");
		META_DefineMember(ParticleSettings, velocityLimitAmount, "VelocityLimitAmount");

		META_DefineMember(ParticleSettings, scaleOverTime, "ScaleOverTime");

		META_DefineMember(ParticleSettings, startRotation, "StartRotation");
		META_DefineMember(ParticleSettings, startRotationVariation, "StartRotationMinMax");
		META_DefineMember(ParticleSettings, rotationRate, "RotationRate");
		META_DefineMember(ParticleSettings, speedScaledRotation, "SpeedScaledRotation");

		META_DefineMember(ParticleSettings, startColor, "StartColor");
		META_DefineMember(ParticleSettings, endColor, "EndColor");
		META_DefineMember(ParticleSettings, texture_resourceID, "TextureResourceID");

		META_DefineMember(ParticleSettings, hasTrail, "HasTrail");
		META_DefineMember(ParticleSettings, trailEmissionRate, "TrailEmissionRate");
		META_DefineMember(ParticleSettings, trailLifetime, "TrailLifetime");
		META_DefineMember(ParticleSettings, trailScale, "TrailScale");
		META_DefineMember(ParticleSettings, trailEndColor, "TrailEndColor");
	}
};

class ParticleSystem 
{
public:
	ParticleSystem();
	ParticleSystem(const ParticleSystem& ps);
	void Render(float dt, glm::vec2 pos, int id);

	// Emitter loops or dies after lifetime
	void SetIsLooping(bool loop) { m_settings.isLooping = true; }

	// Time (in seconds) between each particle spawning
	void SetEmissionRate(float rate) { m_settings.emissionRate = rate; }

	// Particles emitted per emission
	void SetParticlesPerEmission(int amt) { m_settings.particlesPerEmission = amt; }
	
	// Set Burst minimum and maximum amount of particles spawned in each burst (Also set Burst Emission Rate!)
	void SetBurstEmissionParticles(int minParticles, int maxParticles) { m_settings.burstEmission.x = static_cast<float>(minParticles); m_settings.burstEmission.y = static_cast<float>(maxParticles); }
	
	// Set reoccurance rate for burst particles in seconds (Also set Burst Emission Particles!)
	void SetBurstEmissionRate(float repeatRate) { m_settings.burstEmission.z = repeatRate; }

	// Shape particles are emitted in and the scale of that shape, scaled around the center
	void SetEmissionShape(EmissionShape shape, float scaleX, float scaleY, float thickness = 0) { m_settings.emissionShape = shape; m_settings.emissionShapeScale = glm::vec3(scaleX, scaleY, thickness); }
	
	// Simulation space of particles. Local moves all particles relative to emitter, world move particles independantly of emitter
	void SetSimulationSpace(SimulationSpace space) { m_settings.particleSpace = space; }

	// Lifetime of emitter (only applicable if isLooping = false, otherwise it does nothing)
	void SetEmitterLifetime(float life) { m_settings.emitterLifetime = life; }
	
	// Lifetime of particle in seconds, and variation of life of particle, between -Variation/2 and +Variation/2
	void SetParticleLifetime(float life, float varianceMin = 0, float varianceMax = 0) { m_settings.particleLifetime = life; m_settings.particleLifetimeVariance = glm::vec2(varianceMin, varianceMax); }
	
	// Velocity of particle at creation in world units per second, and variation of starting velocity in each direction, between -Variation/2 and +Variation/2
	void SetVelocity(glm::vec2 vel, glm::vec4 variance = glm::vec4(0,0,0,0)) { m_settings.startingVelocity = vel; m_settings.startingVelocityVariance = variance; }
	
	// Acceleration of particle in world units per second per second
	void SetAcceleration(glm::vec2 a) { m_settings.acceleration = a; }
	
	// Particles spawned at start scale and linearly scale to end scale over their lifetime
	void SetScaleOverLife(glm::vec2 startScale, glm::vec2 endScale) { m_settings.scaleOverTime = glm::vec4(startScale, endScale); }
	
	// Start rotation of particle, and variation of start rotation between -Variation/2 and +Variation/2
	void SetStartRotation(float degrees, float variationMin = 0, float variationMax = 0) { m_settings.startRotation = degrees * 3.14159265359f / 180.0f; m_settings.startRotationVariation = glm::vec2(variationMin, variationMax) * 3.14159265359f / 180.0f; }
	
	// Rotation in degrees per second
	void SetRotationRate(float rate) { m_settings.rotationRate = rate; }
	
	// Blend color of particle over lifetime linearly interpolated from start to end over the particle's lifetime
	void SetColor(glm::vec4 start, glm::vec4 end) { m_settings.startColor = start; m_settings.endColor = end; }

	// Set's particle texture. Does not apply to trail particles. Particle color will be blended with texture
	void SetTexture(ResourceID texture) { m_settings.texture_resourceID = texture; }

	// True to spawn trail particles
	void SetHasTrail(bool trail) { m_settings.hasTrail = trail; }
	
	// Emission rate of trail particles, lower for smoother trail (Beware not to spawn too many or else max particle limit will be reached quickly)
	void SetTrailEmissionRate(float rate) { m_settings.trailEmissionRate = rate; }
	
	// Lifetime of trail particles in seconds
	void SetTrailLifetime(float life) { m_settings.trailLifetime = life; }
	
	// Scale of trail particles RELATIVE TO SOURCE PARTICLE (use (1,1) to match scale of particle it trails)
	void SetTrailScale(glm::vec2 scale) { m_settings.trailScale = scale; }

	// Set color of trail, start color is color of trail near particle, end color is color of trail near tail end
	//void SetTrailColor(glm::vec4 start, glm::vec4 end) { m_settings.trailStartColor = start; m_settings.trailEndColor = end; }

	ParticleSettings *GetSettings() { return &m_settings; }

private:
	friend void ImGui_Particles(ParticleSystem *particles, GameObject object, Editor *editor);

	void UpdateParticles(float dt, glm::vec2 pos, int id);
	void RenderParticles(glm::vec2 pos, int id);
	void GenRandomTexture();
	void Resize();

	int m_isFirst = 0; // Is first time rendering
	unsigned int m_currVB = 0; // Current VBO index 
	unsigned int m_currTFB = 1; // Current Transform Feedback index
	
	float m_time = 0; // Time particle has been alive, used for random generation seed in shader
	int m_loops = 0; // Loop counter, used to add randomness to burst emitters that emit at 0
	glm::vec2 lastPos = glm::vec2(0,0);
	bool isLargeSize = false;
	ParticleSettings m_settings; // Particle settings (So much memory!)

	// Buffers
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
	GLuint m_VAO;

	static GLuint m_randTexture;

	META_REGISTER(ParticleSystem)
	{
		META_DefineMember(ParticleSystem, m_settings, "settings");
	}
};
