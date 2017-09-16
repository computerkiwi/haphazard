#include "Camera.h"

#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

///
// Camera
///

Graphics::Camera::Camera()
{
	glGenBuffers(1, &mMatricesUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, mMatricesUbo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // allocate memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mMatricesUbo, 0, 2 * sizeof(glm::mat4));
}


void Graphics::Camera::SetView(glm::vec3 pos, glm::vec3 target, glm::vec3 upVector)
{
	mPosition = pos;
	mCenter = target;
	mUp = upVector;
	ApplyCameraMatrices();
}

void Graphics::Camera::SetProjection(float zoom, float aspectRatio, float near, float far)
{
	m_zoom = zoom;
	mAspectRatio = aspectRatio;
	mNear = near;
	mFar = far;
	ApplyCameraMatrices();
}

#include <cstdio>

void Graphics::Camera::ApplyCameraMatrices()
{
	glm::mat4 view = glm::lookAt(mPosition, mCenter, mUp);
	glm::mat4 proj = glm::ortho(-1.0f * m_zoom, 1.0f * m_zoom, -1.0f * m_zoom / mAspectRatio, 1.0f * m_zoom / mAspectRatio, mNear, mFar);
		//glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);

	glm::mat4 data[] = { view, proj };

	printf("%f,%f\n", proj[0][0], proj[1][1]);

	glBindBuffer(GL_UNIFORM_BUFFER, mMatricesUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::mat4), data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Graphics::Camera::Orbit(float degrees, glm::vec3 axis) // Rotates around target
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, mCenter);
	matrix = glm::rotate(matrix, glm::radians(degrees), axis);
	matrix = glm::translate(matrix, -mCenter);

	glm::vec3 forward = mPosition - mCenter;

	if ((axis.x && forward.x) || (axis.y && forward.y) || (axis.z && forward.z)) // If axis contains forward
	{
		mPosition = matrix * glm::vec4(mPosition, 1);
		mUp = matrix * glm::vec4(mUp, 1);
	}
	else
		mPosition = matrix * glm::vec4(mPosition, 1);

	ApplyCameraMatrices();
}

void Graphics::Camera::OrbitAround(glm::vec3 center, float degrees, glm::vec3 axis) // Rotates around center
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, center);
	matrix = glm::rotate(matrix, glm::radians(degrees), axis);
	matrix = glm::translate(matrix, -center);

	glm::vec3 forward = mPosition - mCenter;

	if ((axis.x && forward.x) || (axis.y && forward.y) || (axis.z && forward.z)) // If axis contains forward
	{
		mPosition = matrix * glm::vec4(mPosition, 1);
		mUp = matrix * glm::vec4(mUp, 1);
	}
	else
		mPosition = matrix * glm::vec4(mPosition, 1);

	ApplyCameraMatrices();
}
