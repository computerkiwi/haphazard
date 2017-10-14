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


	bool m_isFirst = true; // Is first time rendering
	unsigned int m_currVB; // Current 
	unsigned int m_currTFB;
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
};