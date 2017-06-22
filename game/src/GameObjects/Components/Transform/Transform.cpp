/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "Transform.h"

Transform::Transform() : Transform(nullptr, glm::vec2(), glm::vec2(), glm::mat3(), 0.0f)
{
}


Transform::Transform(GameObject * parent) : Transform(parent, glm::vec2(), glm::vec2(), glm::mat3(), 0.0f)
{
}



Transform::Transform(GameObject * parent, glm::vec2 & position, glm::vec2 & scale)
								: Component(parent), mMatrix(glm::mat3()), mPosition(position), mScale(scale), mRotation(0.0f)
{
}


Transform::Transform(GameObject * parent, glm::vec2 && position, glm::vec2 && scale)
								: Transform(parent, position, scale, glm::mat3(), 0.0f)
{
}



Transform::Transform(GameObject * parent, glm::vec2 & position, glm::vec2 & scale, glm::mat3 & matrix, float rotation)
								: Component(parent), mMatrix(matrix), mPosition(position), mScale(scale), mRotation(rotation)
{
}


Transform::Transform(GameObject * parent, glm::vec2 && position, glm::vec2 && scale, glm::mat3 && matrix, float rotation)
								: Component(parent), mMatrix(matrix), mPosition(position), mScale(scale), mRotation(rotation)
{
}



glm::vec2 Transform::GetPosition() const
{
	assert(mParent && "Component does not have a parent!!");
	return mPosition;
}


void Transform::SetPosition(glm::vec2 position)
{
	assert(mParent && "Component does not have a parent!!");
	mPosition = position;
}


glm::vec2 Transform::GetScale() const
{
	assert(mParent && "Component does not have a parent!!");
	return mScale;
}


void Transform::SetScale(glm::vec2 scale)
{
	assert(mParent && "Component does not have a parent!!");
	mScale = scale;
}


const glm::mat3 & Transform::GetMatrix() const
{
	assert(mParent && "Component does not have a parent!!");
	return mMatrix;
}


void Transform::SetMatrix(glm::mat3 & matrix)
{
	assert(mParent && "Component does not have a parent!!");
	mMatrix = matrix;
}


float Transform::GetRotation() const
{
	assert(mParent && "Component does not have a parent!!");
	return mRotation;
}


void Transform::SetRotation(float rotation)
{
	assert(mParent && "Component does not have a parent!!");
	mRotation = rotation;
}


