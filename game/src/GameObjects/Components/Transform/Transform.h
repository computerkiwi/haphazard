/*
FILE: Transform.h
PRIMARY AUTHOR: Sweet

Transform Definitions

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "../Component.h"
#include <glm/glm.hpp>


class Transform : public Component
{
public:
	Transform();
	explicit Transform(GameObject * parent);

	Transform(GameObject * parent, glm::vec3 & position, glm::vec3 & scale);
	Transform(GameObject * parent, glm::vec3 && position, glm::vec3 && scale);

	Transform::Transform(GameObject * parent, glm::vec3 & position, glm::vec3 & scale, float rotation = 0.0f);
	Transform::Transform(GameObject * parent, glm::vec3 && position, glm::vec3 && scale, float rotation = 0.0f);


	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 position);

	glm::vec3 GetScale() const;
	void SetScale(glm::vec3 scale);

	glm::mat4 GetMatrix();

	const glm::vec3 & GetRotation() const;
	void SetRotation(const glm::vec3 & rotation);

private:
	glm::vec3   m_Position;
	glm::vec3   m_Scale;
	glm::vec3   m_Rotation;
	bool        isDirty = true;
};

