/*
FILE: TransformComponent.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta/meta.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include "GameObject.h"

// TODO[Kieran] - Move this somewhere more general.
constexpr float DegToRad(float degrees)
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
					   float rotation = 0.0f);

	//---------
	// Getters
	//---------

	void SetParent(GameObject parent);
	GameObject GetParent() const;

	float& Rotation();

	float GetRotation() const;

	void SetRotation(const float& rotation);

	glm::vec3& Position();

	glm::vec3& GetRelativePosition();

	glm::vec3 GetPosition() const;

	void SetPosition(const glm::vec3& position);

	glm::vec3 Position() const;

	glm::vec2 GetPosition2D() const;

	glm::vec3& Scale();

	glm::vec3 GetScale() const;

	void SetScale(const glm::vec3& scale);

	glm::vec2 Scale2D() const;

	glm::mat4 GetMatrix4() const;

private:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	float m_rotation; // Stored in degrees.

	GameObject m_parent;
	glm::vec3 m_parenting_position;

	META_REGISTER(TransformComponent)
	{
		META_DefineType(TransformComponent);

		META_DefineGetterSetter(TransformComponent, glm::vec3, GetPosition, SetPosition, "position");

		META_DefineGetterSetter(TransformComponent, glm::vec3, GetScale, SetScale, "scale");

		META_DefineGetterSetter(TransformComponent, float, GetRotation, SetRotation, "rotation");
	}
};
