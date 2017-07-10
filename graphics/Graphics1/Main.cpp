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
	Camera main;
	main.SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	main.SetProjection(45.0f, 800.0f / 600.0f, 1, 10);

	//Create screen
	Screen screen(Screen::FX::DEFAULT, 800, 600);

	// Check screen for completeness
	if(!Screen::CurrentScreenIsComplete())
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		int a; std::cin >> a;
		exit(1);
	}

	//Screen mesh 
	ScreenMesh screenMesh(screen);


	float dt = 0.0f, last = 0.0f;
	do //Main Loop
	{
		float currentFrame = (float)glfwGetTime();
		dt = currentFrame - last;
		last = currentFrame;

		glEnableVertexAttribArray(0);

		// Render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, screen.ID()); // Set framebuffer to off screen rendering
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers

		Shaders::defaultShader->Use();

		main.Use();
		//main.OrbitAround(glm::vec3(0, 0.2f, 2.2f), dt * 120, glm::vec3(1, 0, 0));
		main.Orbit(dt * 30, glm::vec3(0, 1, 0));

		//Order matters if they are both transparent
		mesh2.Draw();
		mesh.Draw();

		// Render to screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f); //Set failure to render color 
		glClear(GL_COLOR_BUFFER_BIT);
		Shaders::defaultScreenShader->Use();
		
		screenMesh.Draw();
		/*
		glBindVertexArray(screenVAO);
		//main.SetFOV(20);
		glBindTexture(GL_TEXTURE_2D, screen.ColorBuffer());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		*/

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == false);

	Shaders::Unload();
	
	return 0;
}
