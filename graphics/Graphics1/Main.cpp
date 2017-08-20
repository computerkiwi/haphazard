#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Mesh.h"
#include "Screen.h"
#include "Shaders.h"
#include "Texture.h"
#include "Transform.h"
#include "Settings.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

int Settings::ScreenWidth() { return SCREEN_WIDTH; }
int Settings::ScreenHeight() { return SCREEN_HEIGHT; }
int Settings::AntiAliasing() { return 4; }

GLFWwindow* WindowInit()
{
	std::cout << "    Initializing glfw... " ;

	if (glfwInit() == false)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLFW");
		exit(1);
	}

	std::cout << "Initialized glfw" << std::endl;

	glfwWindowHint(GLFW_SAMPLES, 4); //4 MSAA
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "<3", NULL, NULL);

	std::cout << "    Window created" << std::endl;

	if (!window)
	{
		glfwTerminate();
		fprintf(stderr, "Could not create window");
		int i;
		std::cin >> i;
		exit(1);
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLEW");
		exit(1);
	}
	
	return window;
}

int main()
{
	using namespace Graphics; //Fuck the police

	std::cout << "Initializing window" << std::endl;

	//Init OpenGL and start window
	GLFWwindow *window = WindowInit();
	
	std::cout << "Initializing shaders" << std::endl;

	// Load Shaders
	Shaders::Init();

	Shaders::defaultShader->Use();
	GLuint program = Shaders::defaultShader->GetProgramID();

	// Enable engine default buffer
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE); // Enable multisampling for framebuffer 0 (enabled by default anyway)

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK); //Dont render back, CCW tri vertices

	// Create triangle
	Mesh mesh = Mesh();
	mesh.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-0.5f,  0.5f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1, // Top Left
		-0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f, 1, 0, 0, // Bot Left
		 0.5f, -0.5f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0  // Bot Right
	);
	mesh.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		 0.5f, -0.5f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0, // Bot Right
		 0.5f,  0.5f, 0, 1.0f, 0.0f, 0.0f, 1, 1, 1, // Top Right
		-0.5f,  0.5f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1  // Top Left
	);
	mesh.CompileMesh();
	Texture tex = Texture("sampleBlend.png");
	mesh.SetTexture(tex);


	Mesh mesh2 = Mesh();
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-1.0f, -1.0f, 0, 1.0f, 0.0f, 0.0f, 0.5f, 0, 0, // Bot Left
		 1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 0.5f, 1, 0, // Bot Right
		-1.0f,  1.0f, 0, 0.0f, 0.0f, 1.0f, 0.5f, 0, 1  // Top Left
	);
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		 1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0, // Bot Right
		 1.0f,  1.0f, 0, 1.0f, 0.0f, 0.0f, 1, 1, 1, // Top Right
		-1.0f,  1.0f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1  // Top Left
	);
	mesh2.CompileMesh();
	mesh2.SetTexture(0);
	mesh.SetBlendMode(Graphics::BlendMode::BM_ADDITIVE);

	mesh2.transform.SetPosition({ -0.5f, 0.0f, 0.0f });
	mesh2.transform.SetRotation({ -45.0f, 90, 0 });
	mesh2.transform.SetScale({ 0.5f, 0.5f, 1.0f });

	//Camera
	Camera mainCamera;
	mainCamera.SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	mainCamera.SetProjection(45.0f, ((float)SCREEN_WIDTH) / SCREEN_HEIGHT, 1, 10);


	//Screen settings
	Screen::GetView().SetBackgroundColor(0, 0, 0, 1);
	Graphics::Shaders::ScreenShader::HDR->SetVariable("Exposure", 1);

//	Screen::GetView().AddEffect(Graphics::FX::SHARPEN);
// 	Screen::GetView().AddEffect(Graphics::FX::EDGE_DETECTION);
//	Screen::GetView().AddEffect(Graphics::FX::BLUR);
	Screen::GetView().AddEffect(Graphics::FX::BLOOM);

	float dt = 0.0f, last = 0.0f, currentFrame;
	int frames = 0, lastFrames = 0;
	float second = 1;
	do //Main Loop
	{
		//////////// Frame stuff
		{
			currentFrame = (float)glfwGetTime();
			dt = currentFrame - last;
			last = currentFrame;

			second -= dt;
			frames++;
			if (second <= 0)
			{
				lastFrames = frames;
				frames = 0;
				second = 1;
			}

			std::string title = "<3	    dt: " + std::to_string(dt) + "    fps: " + std::to_string(lastFrames);
			glfwSetWindowTitle(window, title.c_str());
		}
		////////////
		glEnableVertexAttribArray(0);
		Screen::GetView().Use();
		Screen::UpdateRaindrops(dt);
		////Start Loop

		// Don't need to use Camera.Use if there is only one camera being used, or its already enabled
		mainCamera.Orbit(dt * 30, glm::vec3(0, 1, 0));

		//Order matters if they are both transparent
		mesh2.Draw();
		mesh.Draw();

		////End Loop
		glBlendFunc(GL_ONE, GL_ZERO);
		Screen::GetView().Draw();

		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == false);

	Shaders::Unload();
	
	return 0;
}
