/*
FILE: Transform.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta/meta.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>

// TODO[Kieran] - Move this somewhere more general.
static float DegToRad(float degrees)
{
	const float PI = 3.14159265358979323846264338327950f;
	const float RADIANS_IN_CIRCLE = 2 * PI;
	const float DEGREES_IN_CIRCLE = 360.0f;

	return degrees * RADIANS_IN_CIRCLE / DEGREES_IN_CIRCLE;
}

class TransformComponent
{
public:
	//--------------
	// Constructors
	//--------------

	TransformComponent(const glm::vec3& position = glm::vec3(0.0f), 
	                   const glm::vec3& scale = glm::vec3(1.0f), 
	                   float rotation = 0.0f) 
	                 : m_position(position), m_scale(scale), m_rotation(rotation)
	{
	}

	//---------
	// Getters
	//---------

	float& GetRotation()
	{
		return m_rotation;
	}

	float GetRotation() const
	{
		return m_rotation;
	}

	void SetRotation(const float& rotation)
	{
		m_rotation = rotation;
	}

	glm::vec3& Position()
	{
		return m_position;
	}

	glm::vec3 GetPosition() const
	{
		return m_position;
	}

	void SetPosition(const glm::vec3& position)
	{
		m_position = position;
	}

	glm::vec3 Position() const
	{
		return m_position;
	}

	glm::vec2 GetPosition2D()
	{
		return m_position;
	}

	glm::vec2 GetPosition2D() const
	{
		return m_position;
	}

	glm::vec3& GetScale()
	{
		return m_scale;
	}

	glm::vec3 GetScale() const
	{
		return m_scale;
	}

	glm::vec3 GetScale() const
	{
		return m_scale;
	}

	void SetScale(const glm::vec3& scale)
	{
		m_scale = scale;
	}

	glm::vec2 Scale2D() const
	{
		return m_scale;
	}

	glm::mat4 GetMatrix4() const
	{
		return glm::translate(glm::mat4(),m_position) * glm::rotate(glm::mat4(), DegToRad(m_rotation), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(), m_scale);
	}

private:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	float m_rotation; // Stored in degrees.

	META_REGISTER(TransformComponent)
	{
		META_DefineType(TransformComponent);

		META_DefineGetterSetter(TransformComponent, glm::vec3, GetPosition, SetPosition, "position");

		META_DefineGetterSetter(TransformComponent, glm::vec3, GetScale, SetScale, "scale");

		META_DefineGetterSetter(TransformComponent, float, GetRotation, SetRotation, "rotation");
	}
};
