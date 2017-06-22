/*
FILE: Transform.h
PRIMARY AUTHOR: Sweet

Transform Definitions

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "../Component.h"
#include <glm/mat3x3.hpp>


class Transform : public Component
{
public:
	Transform();
	explicit Transform(GameObject * parent);

	Transform(GameObject * parent, glm::vec2 & position, glm::vec2 & scale);
	Transform(GameObject * parent, glm::vec2 && position, glm::vec2 && scale);

	Transform::Transform(GameObject * parent, glm::vec2 & position, glm::vec2 & scale, glm::mat3 & matrix, float rotation = 0.0f);
	Transform::Transform(GameObject * parent, glm::vec2 && position, glm::vec2 && scale, glm::mat3 && matrix, float rotation = 0.0f);


	glm::vec2 GetPosition() const;
	void SetPosition(glm::vec2 position);

	glm::vec2 GetScale() const;
	void SetScale(glm::vec2 scale);

	const glm::mat3 & GetMatrix() const;
	void SetMatrix(glm::mat3 & matrix);

	float GetRotation() const;
	void SetRotation(float rotation);

private:
	glm::mat3   mMatrix;
	glm::vec2   mPosition;
	glm::vec2   mScale;
	float       mRotation;
};

