/*******************************************************
FILE: Input.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "graphics\Camera.h"
#include "graphics\Settings.h"

#include "Input.h"
#include <iostream>


#include <imgui.h>
#include "Imgui\imgui-setup.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Engine/Physics/RigidBody.h"

#include "Engine/Engine.h"

namespace Input
{
  #define MAX_KEY_SIZE 350 // Maximum number of keys on keyboard and mouse
  #define MAX_PLAYERS 4    // Maximum number of players

  // Vector of keys available
  typedef std::vector<int> InputData;

  ////////// Static Variables //////////
  static GLFWwindow * inputWindow;         // Window to detect input from
  static glm::vec2 cursorPos;              // x, y cursor positions; top-left is origin
  static glm::vec2 prevCursorPos;          // x, y of cursor last frame
  static std::vector<Gamepad *> gamepads;  // TEMP: vector of gamepad objects
  static glm::vec2 currScroll;             // x, y mouse wheel offset (x is 0 coz vertical)
  static glm::vec2 prevScroll;             // prev x, y mouse wheel state
  static int scrollCount;                  // Number of scroll wheel ticks
  static int scrollDir; 

  static InputData prevState(MAX_KEY_SIZE); // Holds previous key states
  static InputData currState(MAX_KEY_SIZE); // Holds current key states
  static InputData nextState(MAX_KEY_SIZE); // Holds next key states collected by callback


  // Initializes input window for detection
  void Init(GLFWwindow * window)
  {
    // Check for window
    if (window)
    {
      inputWindow = window;
    }
    else
    {
      std::cout << "No input window set" << std::endl;
    }

    // Callbacks for GLFW
    glfwSetKeyCallback(inputWindow, KeyCallback);                  // Keyboard
    glfwSetCursorPosCallback(inputWindow, CursorCallback);         // Cursor position
    glfwSetMouseButtonCallback(inputWindow, MouseButtonCallback);  // Mouse buttons
    glfwSetJoystickCallback(GamepadCallback);                      // Gamepad connectivity
    glfwSetScrollCallback(inputWindow, ScrollCallback);            // Mouse wheel

    // Allocate memory for gamepads
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
      gamepads.push_back(new Gamepad(i));

      if (glfwJoystickPresent(i))
      {
        gamepads[i]->EnableGamepad();
      }
    }

    scrollCount = 0;
    scrollDir = 0;
  }

  // Calls GLFW functions to check and store input
  void Update()
  {
    // Callback functions always called first; key states stored in "next state"

    // Read gamepad input
    UpdateGamepads();

    // Copy over current to previous states
    UpdateKeyStates();
    UpdateScroll();
  }

  // Screen coordinates
  glm::vec2 GetMousePos()
  {
    return cursorPos;
  }

  // World coordinates
  glm::vec2 GetMousePos_World()
  {
    return ScreenToWorld(cursorPos);
  }


  void RecordMousePos()
  {
	  prevCursorPos = cursorPos;
  }

  glm::vec2 GetPrevMousePos()
  {
	  return prevCursorPos;
  }

  glm::vec2 GetPrevMousePos_World()
  {
	  return ScreenToWorld(prevCursorPos);
  }

  // Upper left is (0,0)
  glm::vec2 ScreenToWorld(glm::vec2 cursor)
  {
	Camera *cam = Camera::GetActiveCamera();

	// Get the Mouse in screen coordinates, these range from -1 to 1
	glm::vec2 screenPos = glm::vec2((cursor.x / (Settings::ScreenWidth() / 2)) - 1.0f, 1.0f - (cursor.y / (Settings::ScreenHeight() / 2)));

	// Get the view and projection matrices from the camera
	glm::mat4 view = glm::lookAt(cam->m_Position, cam->m_Center, cam->m_Up);
	glm::mat4 proj = glm::ortho(-1.0f * cam->m_Zoom, 1.0f * cam->m_Zoom, -1.0f * cam->m_Zoom / cam->m_AspectRatio, 1.0f * cam->m_Zoom / cam->m_AspectRatio, cam->m_Near, cam->m_Far);

	// Get the matrix needed to undo the camera matrix
	glm::mat4 matrix = glm::inverse(proj * view);

	// Calculate and save the world coordinates
	glm::vec4 cPos = matrix * glm::vec4(screenPos, 0.0f, 1);

    return cPos;
  }

  glm::vec2 ScreenPercentToWorld(glm::vec2 screenPos)
  {
	  Camera *cam = Camera::GetActiveCamera();

	  // Get the view and projection matrices from the camera
	  glm::mat4 view = glm::lookAt(cam->m_Position, cam->m_Center, cam->m_Up);
	  glm::mat4 proj = glm::ortho(-1.0f * cam->m_Zoom, 1.0f * cam->m_Zoom, -1.0f * cam->m_Zoom / cam->m_AspectRatio, 1.0f * cam->m_Zoom / cam->m_AspectRatio, cam->m_Near, cam->m_Far);

	  // Get the matrix needed to undo the camera matrix
	  glm::mat4 matrix = glm::inverse(proj * view);

	  // Calculate and save the world coordinates
	  glm::vec4 cPos = matrix * glm::vec4(screenPos, 0.0f, 1);

	  return cPos;
  }

  // Check if key is pressed; takes the key to check
  // Returns true if pressed
  bool IsPressed(Key key)
  {
    int i = static_cast<int>(key);

    if ((prevState[i] == KeyState::Released) &&
        (currState[i] == KeyState::Pressed))
    {
      return true;
    }

    // Key not pressed
    return false;
  }
  
  // Check if key is held down; takes the key to check
  // Returns true if held down
  bool IsHeldDown(Key key)
  {
    int i = static_cast<int>(key);

    if (currState[i] != KeyState::Released)
    {
      return true;
    }

    // Key not held down
    return false;
  }

  // Check if key has been released; takes key to check
  // Returns true if released
  bool IsReleased(Key key)
  {
    int i = static_cast<int>(key);

    if ((prevState[i] != KeyState::Released) &&
        (currState[i] == KeyState::Released))
    {
      return true;
    }

    // Key not released
    return false;
  }

  void Exit()
  {
    // Free gamepads
    for (size_t i = 0; i < gamepads.size(); ++i)
    {
      if (gamepads[i] != NULL)
      {
        delete gamepads[i];
      }
    }
  }

  void SetGamepad()
  {

  }

  // Debug to check for key states and cursor coordinates
  void InputDebug(Key key1, Key key2, Key key3)
  {
    char letter1 = static_cast<int>(key1);
    char letter2 = static_cast<int>(key2);
    char letter3 = static_cast<int>(key3);

    if (IsPressed(key1) == true)
    {
      printf("%c is pressed\n", letter1);
    }
    else if (IsHeldDown(key2) == true)
    {
      printf("%c is held down\n", letter2);
    }
    else if (IsReleased(key3) == true)
    {
      printf("%c is released\n", letter3);
    }

    std::cout << "x curr scroll: " << currScroll.x << std::endl;
    std::cout << "y curr scroll: " << currScroll.y << std::endl;
    std::cout << "x prev scroll: " << prevScroll.x << std::endl;
    std::cout << "y prev scroll: " << prevScroll.y << std::endl;
    std::cout << "count: " << scrollCount << std::endl;
  }

  // Callback for GLFW keyboard input detection
  // Stores keyboard input information to be set as Current State in next frame
  void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
  {
    // Key unrecognized, do nothing
    if (key == static_cast<int>(Key::Unknown))
    {
      return;
    }

    if (key)
    {
      switch (action)
      {
        // Key was pressed
        case GLFW_PRESS:
          SetKeyState(key, KeyState::Pressed);
          break;

        // Key was held down
        case GLFW_REPEAT:
          SetKeyState(key, KeyState::Pressed);
          break;

        // Key was released
        case GLFW_RELEASE:
          SetKeyState(key, KeyState::Released);
      }

    // Noah does a thing
	  ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    }
  }

  // Stores mouse position
  void CursorCallback(GLFWwindow * window, double xpos, double ypos)
  {
    cursorPos.x = (float)xpos;
    cursorPos.y = (float)ypos;
  }

  // Stores mouse button input information
  void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod)
  {
    // Button was pressed
    if (button >= 0)
    {
      // Mouse button pressed
      if (action == KeyState::Pressed)
      {
        SetKeyState(button, KeyState::Pressed);
      }
      // Mouse button released
      else
      {
        SetKeyState(button, KeyState::Released);
      }
    }
  }

  // Detects when gamepad is connected
  void GamepadCallback(int joy, int event)
  {
    // Joystick connected
    if (event == GLFW_CONNECTED)
    {
      std::cout << "Gamepad connected" << std::endl;

      // Enable gamepad
      gamepads[joy]->EnableGamepad();

    }
    // Joystick disconnected
    else if (event == GLFW_DISCONNECTED)
    {
      std::cout << "Gamepad disconnected" << std::endl;
      
      // Disable gamepad
      gamepads[joy]->DisableGamepad();
    }
  }

  // Detects mouse wheel
  static void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset)
  {
    // Store scroll data
    currScroll.x = static_cast<float>(xOffset);
    currScroll.y = static_cast<float>(yOffset);

    // Increment counter until direction changes
    // TODO: Frames might be too fast, fix it?
    if (yOffset == prevScroll.y)
    {
      ++scrollCount;
    }
    else
    {
      scrollCount = 0;
    }

	  ImGui_ImplGlfwGL3_ScrollCallback(window, xOffset, yOffset);
  }

  glm::vec2 GetScroll()
  {
    // y is 1 (up) or -1 (down)
    return prevScroll;
  }

  // Number of scroll tick
  int GetScrollCount()
  {
    return scrollCount;
  }

  void UpdateScroll()
  {
    // Copy scroll data
    prevScroll.x = currScroll.x;
    prevScroll.y = currScroll.y;

    // Reset scroll
    currScroll.x = 0;
    currScroll.y = 0;
  }

  // Helper function to delay update until input is read
  void SetKeyState(int key, KeyState state)
  {
    // Sets next state
    nextState[key] = state;
  }

  // Update previous state with current state
  // Sets current state to next state from callback
  void UpdateKeyStates()
  {
    // Loops through all keys
    for (int i = 0; i < MAX_KEY_SIZE; ++i)
    {
      prevState[i] = currState[i];
      currState[i] = nextState[i];
    }
  }

  void UpdateGamepads()
  {
    // Update each gamepad
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
      if (gamepads[i]->IsConnected() == true)
      {
        gamepads[i]->Update();
      }
    }
  }

  // Gets value for specific gamepad axis
  float GamepadGetAxis(PlayerID player, GamepadAxis axis)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->GetGamepadAxis(axis);
    }

    return 0.0f;
  }

  // Checks if gamepad button is pressed
  bool GamepadIsPressed(PlayerID player, GamepadButton button)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->IsPressed(button);
    }

	return false;
  }

  // Checks if gamepad button is held down
  bool GamepadIsHeldDown(PlayerID player, GamepadButton button)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->IsHeldDown(button);
    }

	return false;
  }

  // Checks if gamepad button is released
  bool GamepadIsReleased(PlayerID player, GamepadButton button)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->IsReleased(button);
    }

	return false;
  }

  // Returns number of gamepads connected
  int GamepadsConnected()
  {
    int players = 0;

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
      if (gamepads[i]->IsConnected() == true)
      {
        ++players;
      }
    }

    return players;
  }
}
