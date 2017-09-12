#pragma once

#include "GL\glew.h"
#include <glm/glm.hpp>

namespace Graphics
{

	class Camera
	{
	public:
		Camera();

		void SetView(glm::vec3 pos, glm::vec3 target, glm::vec3 upVector);
		void SetProjection(float FOV, float aspectRatio, float near, float far);

		void SetPosition(glm::vec3 pos) { mPosition = pos; ApplyCameraMatrices(); }
		void SetTarget(glm::vec3 pos) { mCenter = pos; ApplyCameraMatrices(); }
		void SetUp(glm::vec3 up) { mUp = up; ApplyCameraMatrices(); }

		void SetFOV(float fovDegrees) { mFOV = fovDegrees; ApplyCameraMatrices(); }
		void SetAspectRatio(float ar) { mAspectRatio = ar; ApplyCameraMatrices(); }
		void SetNearPlane(float near) { mNear = near; ApplyCameraMatrices(); }
		void SetFarPlane(float far) { mFar = far; ApplyCameraMatrices(); }

		void MoveCamera(glm::vec3 pos)
		{
			mCenter += pos - mPosition;
			mPosition = pos;
			ApplyCameraMatrices();
		}

		glm::vec3 GetPosition() { return mPosition; }
		float GetFOV() { return mFOV; }

		void Orbit(float degrees, glm::vec3 axis); // Rotates around target
		void OrbitAround(glm::vec3 center, float degrees, glm::vec3 axis); // Rotates around center

		void Use() { ApplyCameraMatrices(); }

	private:
		void ApplyCameraMatrices();

		//View matrix
		glm::vec3 mPosition = glm::vec3(0, 0, 5.0f);
		glm::vec3 mCenter = glm::vec3(0, 0, 0);
		glm::vec3 mUp = glm::vec3(0, 0, 1);

		//Projection matrix
		float mFOV = 45.0f;
		float mAspectRatio;
		float mNear = 1.0f;
		float mFar = 10.0f;

		GLuint mMatricesUbo;
	};

};