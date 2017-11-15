#version 330 core

layout (points) in;
layout (points, max_vertices = 30) out;

in float PType[];
in vec2 PPos[];
in vec2 PVel[];
in vec2 PScale[];
in float PRot[];
in float PLife[];
in float PMaxLife[];

out float Type;
out vec2 Position;
out vec2 Velocity;
out vec2 Scale;
out float Rotation;
out float Life;
out float MaxLife;


/// Enum Defines \\\

#define EMITTER_TYPE 0.0f
#define PARTICLE_TYPE 1.0f
#define TRAIL_TYPE 2.0f

#define SHAPE_POINT	        0.0f
#define SHAPE_CIRCLE_VOLUME 1.0f
#define SHAPE_CIRCLE_EDGE   2.0f
#define SHAPE_SQUARE_VOLUME 3.0f
#define SHAPE_SQUARE_EDGE   4.0f

#define SPACE_WORLD 0.0f
#define SPACE_LOCAL 1.0f

/// Uniforms \\\

layout (std140) uniform UpdateSettings
{
	float	dt;
	float	Time;
	
	float	IsLooping;
	float	EmissionRate;
	float	ParticlesPerEmission;
	vec3	BurstEmission;			// (Amt Min, Amt Max, Reoccurance Rate)
	float	EmissionShape;
	vec2	EmissionShapeScale;

	float	EmitterLifetime;
	float	ParticleLifetime;
	float	ParticleLifetimeVariance;

	vec2	StartingVelocity;
	vec2	StartingVelocityVariance;
	vec2	Acceleration;

	vec4	ScaleOverTime;

	float	StartRotation;
	float	StartRotationVariation;
	float	RotationRate;

	float	HasTrail;
	float	TrailEmissionRate;
	float	TrailLifetime;
	vec2	TrailScale;

	float	SimulationSpace;
	vec2	EmitterPosition;
};

uniform sampler1D RandomTexture;


vec3 rand(float TexCoord)
{
    return texture(RandomTexture, 1.0f/(TexCoord+1) * (Time + dt) ).xyz;
}


vec2 NewParticlePosition(vec2 center, vec2 r)
{
	if(EmissionShape == SHAPE_POINT)
		return center;

	if(EmissionShape < SHAPE_SQUARE_VOLUME)
	{
		// random between 0-2pi
		float angle = (r.x + 0.5f) * 6.28318531;

		// Cirlce Edge
		if(EmissionShape == SHAPE_CIRCLE_EDGE)
			return center + vec2( EmissionShapeScale.x * cos(angle), EmissionShapeScale.y * sin(angle));

		// Circle Volume (Circle edge * random value between 0-1)
		return center + (r.y + 0.5f) * vec2( EmissionShapeScale.x * cos(angle), EmissionShapeScale.y * sin(angle));
	}

	// Square (Disabling volume/edge until needed because math)			//if(EmissionShape == SHAPE_SQUARE_VOLUME)
	return center + r * EmissionShapeScale;
}

void HandleEmitter()
{
	float currAge = PLife[0] + dt;

	// Emit this particle emitter
    Type = EMITTER_TYPE;
    Position = EmitterPosition;
    Velocity = PVel[0];
    Life = currAge;
    EmitVertex();
    EndPrimitive();


	// Handle Emission

	if(IsLooping == 0 && currAge >= EmitterLifetime) 
	{
		// Kill emitter from emitting new particles
		return; 
	}

	// Standard Emission
	if(EmissionRate > 0.0f)
	{
		if ( int(currAge / EmissionRate) > int(PLife[0] / EmissionRate))
		{
			// Emit new particle(s)

			for(int i = 0; i < ParticlesPerEmission; i++)
			{
				vec3 r = rand(i)*2 - vec3(1,1,1);
				vec3 r1 = rand(i*1.5)*2 - vec3(1,1,1);

				if(SimulationSpace == SPACE_LOCAL)
					Position = NewParticlePosition(PPos[0], r.xy) - EmitterPosition;
				else
					Position = NewParticlePosition(PPos[0], r.xy);

				Type = PARTICLE_TYPE;
				Velocity = StartingVelocity + vec2(StartingVelocityVariance.x * r1.x, StartingVelocityVariance.y * r1.y);
				Scale = ScaleOverTime.xy;
				Rotation = StartRotation + StartRotationVariation * (r.z-0.5f);
				MaxLife = ParticleLifetime + ParticleLifetimeVariance * r.z;
				Life = 0;

				EmitVertex();
				EndPrimitive();
			}
		}
	}

	// Burst Emission
	if(BurstEmission.z > 0.0f)
	{
		if( int(currAge / BurstEmission.z) > int(PLife[0] / BurstEmission.z) )
		{
			float amt = rand(0).x * (BurstEmission.y - BurstEmission.x) + BurstEmission.x;
			for(int i = 0; i < amt; i++)
			{
				vec3 r = rand(i)*2 - vec3(1,1,1);
				vec3 r1 = rand(i*1.5)*2 - vec3(1,1,1);

				Type = PARTICLE_TYPE;
				Position = NewParticlePosition(PPos[0], r.xy);
				if(SimulationSpace == SPACE_LOCAL)
					Position = NewParticlePosition(PPos[0], r.xy) - EmitterPosition;
				else
					Position = NewParticlePosition(PPos[0], r.xy);

				Velocity = StartingVelocity + vec2(StartingVelocityVariance.x * r1.x, StartingVelocityVariance.y * r1.y);
				Scale = ScaleOverTime.xy;
				Rotation = StartRotation + StartRotationVariation * (r.z-0.5f);
				MaxLife = ParticleLifetime + ParticleLifetimeVariance * r.z;
				Life = 0;

				EmitVertex();
				EndPrimitive();
			}
		}
	}
}

void HandleParticle()
{
	float currAge = PLife[0] + dt;

	if(PLife[0] < PMaxLife[0]) 
	{
		
		// Still alive, update then emit self
	
		Type = PType[0];
		Position = PPos[0] + PVel[0] * dt;
	    Velocity = PVel[0] + Acceleration * dt;
	    Life = currAge;
		MaxLife = PMaxLife[0];
		Scale = ScaleOverTime.xy * (1 - PLife[0]/PMaxLife[0]) + ScaleOverTime.zw * (PLife[0]/PMaxLife[0]);
		Rotation = PRot[0] + RotationRate*dt;
	    EmitVertex();
	    EndPrimitive();
		
		if(HasTrail != 0
			&& PType[0] == PARTICLE_TYPE 
			&& ( int(currAge / TrailEmissionRate) > int(PLife[0] / TrailEmissionRate)))
		{
			// Spawn Trail particles
			Type = TRAIL_TYPE;
			Scale = TrailScale * PScale[0];
		    Velocity = Scale; // Velocity is used to store initial scale for future scale calculations
			MaxLife = PLife[0] / PMaxLife[0]; // MaxLife is used to hold parent life percent
			Life = 0;
			// Keep Position and Rotation as source

		    EmitVertex();
		    EndPrimitive();
		}
	}
}

void HandleTrailParticle()
{
	float currAge = PLife[0] + dt;

	if(currAge < TrailLifetime)
	{
		Type = PType[0];
		Position = PPos[0];
		Life = currAge;
		MaxLife = PMaxLife[0];
		Velocity = PVel[0];
		Rotation = PRot[0];

		// Velocity is used to store initial scale
		Scale = PScale[0] - PVel[0] * dt / TrailLifetime;

		EmitVertex();
		EndPrimitive();
	}
}

void main()
{
	if(PType[0] == EMITTER_TYPE)
	{
		HandleEmitter();
	}
	else if(PType[0] == PARTICLE_TYPE)
	{
		HandleParticle();
	}
	else
	{
		HandleTrailParticle();
	}
}
