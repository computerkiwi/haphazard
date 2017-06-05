/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "Transform.h"

Transform::Transform() : Transform(glm::vec2(), glm::vec2(), glm::mat3(), 0.0f)
{
}



Transform::Transform(glm::vec2 & position, glm::vec2 & scale) 
								: mMatrix(glm::mat3()), mPosition(position), mScale(scale), mRotation(0.0f)
{
}


Transform::Transform(glm::vec2 && position, glm::vec2 && scale) 
								: mMatrix(glm::mat3()), mPosition(position), mScale(scale), mRotation(0.0f)
{
}



Transform::Transform(glm::vec2 & position, glm::vec2 & scale, glm::mat3 & matrix, float rotation) 
								: mMatrix(matrix), mPosition(position), mScale(scale), mRotation(rotation)
{
}


Transform::Transform(glm::vec2 && position, glm::vec2 && scale, glm::mat3 && matrix, float rotation)
								: mMatrix(matrix), mPosition(position), mScale(scale), mRotation(rotation)
{
}



glm::vec2 Transform::GetPosition() const
{
	return mPosition;
}


void Transform::SetPosition(glm::vec2 position)
{
	mPosition = position;
}


glm::vec2 Transform::GetScale() const
{
	return mScale;
}


void Transform::SetScale(glm::vec2 scale)
{
	mScale = scale;
}


const glm::mat3 & Transform::GetMatrix() const
{
	return mMatrix;
}


void Transform::SetMatrix(glm::mat3 & matrix)
{
	mMatrix = matrix;
}


float Transform::GetRotation() const
{
	return mRotation;
}


void Transform::SetRotation(float rotation)
{
	mRotation = rotation;
}


