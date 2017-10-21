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


	int m_isFirst = 0; // Is first time rendering
	unsigned int m_currVB = 0; // Current VBO index 
	unsigned int m_currTFB = 1; // Current Transform Feedback index

	// Buffers
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
	GLuint m_VAO;
};