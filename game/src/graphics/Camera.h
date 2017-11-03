/*
FILE: Camera.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include "GL\glew.h"
#include <glm/glm.hpp>

class Camera
{
public:

	///
	// Basic Camera functionality
	///

	Camera();
	void Use() { m_CurrActiveCamera = this; ApplyCameraMatrices(); }

	///
	// Setters
	///

	void SetView(glm::vec3 pos, glm::vec3 target, glm::vec3 upVector);
	void SetProjection(float zoom, float aspectRatio, float near, float far);

	void SetPosition(glm::vec2 pos) { m_Position = m_Center = glm::vec3(pos, 2); m_Center.z = 0; }
	void SetUp(glm::vec3 up) { m_Up = up; }

	void SetRotation(float degrees);

	void SetZoom(float zoom) { m_Zoom = zoom; }
	void SetAspectRatio(float ar) { m_AspectRatio = ar; }
	void SetNearPlane(float near) { m_Near = near; }
	void SetFarPlane(float far) { m_Far = far; }

	///
	// Getters
	///

	glm::vec2 GetPosition() { return glm::vec2(m_Position.x, m_Position.y); }
	float GetRotation() { return m_Rotation; }
	float GetZoom() { return m_Zoom; }

	static Camera* GetActiveCamera() { return m_CurrActiveCamera; }

	///
	// Unique Movement
	///

	void Orbit(float degrees, glm::vec3 axis); // Rotates around target
	void OrbitAround(glm::vec3 center, float degrees, glm::vec3 axis); // Rotates around center

private:
	void ApplyCameraMatrices();

	static Camera* m_CurrActiveCamera;

	//View matrix
	glm::vec3 m_Position = glm::vec3(0, 0, 5.0f);
	glm::vec3 m_Center = glm::vec3(0, 0, 0);
	glm::vec3 m_Up = glm::vec3(0, 0, 1);

	float m_Rotation = 0;

	//Projection matrix
	float m_Zoom = 3.0f;
	float m_AspectRatio;
	float m_Near = 1.0f;
	float m_Far = 10.0f;

	// Uniform buffer object location
	GLuint m_MatricesUbo;

	META_REGISTER(Camera)
	{
		META_DefineType(Camera);
	}
};

