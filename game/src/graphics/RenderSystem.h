#pragma once
#include "GameObjectSystem\GameSpace.h"

struct GLFWwindow;

class RenderSystem : public SystemBase
{
public:
	RenderSystem(GLFWwindow* window);

	virtual void Init();

	// Called each frame.
	virtual void Update(float dt);

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority();

	static void ResizeWindowEvent(GLFWwindow* window, int width, int height);

private:
	GLFWwindow* m_window;
};
