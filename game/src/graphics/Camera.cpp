/*
FILE: Camera.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Camera.h"
#include "Shaders.h"
#include "RenderSystem.h"

#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

///
// Camera
///

Camera* Camera::m_CurrActiveCamera = nullptr;

GLuint Camera::m_MatricesUbo = 0;

Camera::Camera()
{
	if (m_MatricesUbo == 0)
	{
		glGenBuffers(1, &m_MatricesUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesUbo);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // allocate memory
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_MatricesUbo, 0, 2 * sizeof(glm::mat4));
	
		GLuint blockIndex = glGetUniformBlockIndex(Shaders::spriteShader->GetProgramID(), "Matrices");
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_MatricesUbo); // Set this location to 1
		glUniformBlockBinding(Shaders::spriteShader->GetProgramID(), blockIndex, 1);
	}

	// Give us a valid position.
	SetPosition(glm::vec2(0, 0));

	if (m_CurrActiveCamera == nullptr)
		Use();
}

Camera::Camera(const Camera & other) : m_Position(other.m_Position), m_Center(other.m_Position), 
                                       m_Up(other.m_Up), m_Rotation(other.m_Rotation), 
	                                     m_Zoom(other.m_Zoom), m_AspectRatio(other.m_AspectRatio), 
	                                     m_Near(other.m_Near), m_Far(other.m_Far)
{
	// We become the active cam if we're copying an active cam.
	SetIsActiveCam(other.IsActiveCam());
}

Camera::~Camera()
{
	if (m_CurrActiveCamera == this)
	{
		m_CurrActiveCamera = nullptr;
	}
}


void Camera::SetView(glm::vec3 pos, glm::vec3 target, glm::vec3 upVector)
{
	m_Position = pos;
	m_Center = target;
	m_Up = upVector;
}

void Camera::SetProjection(float zoom, float aspectRatio, float near, float far)
{
	m_Zoom = zoom;
	m_AspectRatio = aspectRatio;
	m_Near = near;
	m_Far = far;
}

void Camera::SetRotation(float degrees)
{
	m_Rotation = degrees;
	float r = degrees / 180.0f * 3.1416f;
	glm::mat4 rotation;
	rotation = glm::rotate(rotation, r, glm::vec3(0, 0, 1));
	m_Up = rotation * glm::vec4(0,1,0,1);
}

void Camera::ApplyCameraMatrices()
{
	glm::mat4 view = glm::lookAt(m_Position, m_Center, m_Up);
	glm::mat4 proj = glm::ortho(-1.0f * m_Zoom, 1.0f * m_Zoom, -1.0f * m_Zoom / m_AspectRatio, 1.0f * m_Zoom / m_AspectRatio, m_Near, m_Far);
		//glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);

	glm::mat4 data[] = { view, proj };

	glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::mat4), data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::Orbit(float degrees, glm::vec3 axis) // Rotates around target
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, m_Center);
	matrix = glm::rotate(matrix, glm::radians(degrees), axis);
	matrix = glm::translate(matrix, -m_Center);

	glm::vec3 forward = m_Position - m_Center;

	if ((axis.x && forward.x) || (axis.y && forward.y) || (axis.z && forward.z)) // If axis contains forward
	{
		m_Position = matrix * glm::vec4(m_Position, 1);
		m_Up = matrix * glm::vec4(m_Up, 1);
	}
	else
		m_Position = matrix * glm::vec4(m_Position, 1);
}

void Camera::OrbitAround(glm::vec3 center, float degrees, glm::vec3 axis) // Rotates around center
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, center);
	matrix = glm::rotate(matrix, glm::radians(degrees), axis);
	matrix = glm::translate(matrix, -center);

	glm::vec3 forward = m_Position - m_Center;

	if ((axis.x && forward.x) || (axis.y && forward.y) || (axis.z && forward.z)) // If axis contains forward
	{
		m_Position = matrix * glm::vec4(m_Position, 1);
		m_Up = matrix * glm::vec4(m_Up, 1);
	}
	else
		m_Position = matrix * glm::vec4(m_Position, 1);
}


void Camera::SetNearPlane(float near)
{
	m_Near = near;
}


void Camera::SetFarPlane(float far)
{
	m_Far = far;
}
