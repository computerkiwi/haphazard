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

#include "Graphics.h"
#include "Shaders.h"

GLFWwindow* WindowInit()
{
	if (glfwInit() == false)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLFW");
		exit(1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //4 AA
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "<3", NULL, NULL);

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

	//Init OpenGL and start window
	GLFWwindow *window = WindowInit();
	
	// Load Shaders
	Shaders::Init();

	Shaders::defaultShader->Use();
	GLuint program = Shaders::defaultShader->GetProgramID();

	// Enable engine default buffer
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK); //Dont render back, CCW tri vertices

	// Create triangle
	Mesh mesh = Mesh(Shaders::defaultShader);
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
	mesh.SetTexture(&tex);


	Mesh mesh2 = Mesh(Shaders::defaultShader);
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-1.0f, -1.0f, 0, 1.0f, 0.0f, 0.0f, 1, 0, 0, // Bot Left
		 1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0, // Bot Right
		-1.0f,  1.0f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1  // Top Left
	);
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		 1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0, // Bot Right
		 1.0f,  1.0f, 0, 1.0f, 0.0f, 0.0f, 1, 1, 1, // Top Right
		-1.0f,  1.0f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1  // Top Left
	);
	mesh2.CompileMesh();
	mesh2.SetTexture(NULL);


	mesh2.transform.SetPosition({ -0.5f, 0.0f, 0.0f });
	mesh2.transform.SetRotation({ -45.0f, 90, 0 });
	mesh2.transform.SetScale({ 0.5f, 0.5f, 1.0f });

	//Camera
	Camera mainCamera;
	mainCamera.SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	mainCamera.SetProjection(45.0f, 800.0f / 600.0f, 1, 10);

	//Create screen
	Screen screen(Screen::FX::DEFAULT, 800, 600);
	Screen sharpenScreen(Screen::FX::SHARPEN, 800, 600);
	Screen edgeDectScreen(Screen::FX::EDGE_DETECTION, 800, 600);
	Screen blurScreen(Screen::FX::BLUR, 800, 600);

	// Check screen for completeness
	if(!Screen::CurrentScreenIsComplete())
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		int a; std::cin >> a;
		exit(1);
	}

	//Screen mesh 
	ScreenMesh fullScreenMesh(0,1,0,1);

	ViewScreen::GetViewScreen().SetBackgroundColor(1, 0, 1, 1); // Set failure to render color

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
		////Start Loop


		// Render to screen (pre-effects, raw verts/frags) 
		screen.Use();

		// Don't need to use Camera.Use if there is only one camera being used, or its already enabled
		//mainCamera.Use();
		mainCamera.Orbit(dt * 30, glm::vec3(0, 1, 0));

		//Order matters if they are both transparent
		mesh2.Draw();
		mesh.Draw();

		// Render to view port

		blurScreen.SetIntensity(2);
		fullScreenMesh.CompoundScreens(3, screen, edgeDectScreen, blurScreen);

		ViewScreen::Use();
		fullScreenMesh.Draw(blurScreen);


		////End Loop
		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == false);

	Shaders::Unload();
	
	return 0;
}
