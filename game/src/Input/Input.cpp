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

namespace Input
{
  #define MAX_KEY_SIZE 350 // Maximum number of keys on keyboard and mouse
  #define MAX_PLAYERS 4    // Maximum number of players

  // Vector of keys available
  typedef std::vector<int> InputData;

  ////////// Static Variables //////////
  static GLFWwindow * inputWindow;         // Window to detect input from
  static glm::vec2 cursorPos;              // x, y cursor positions; top-left is origin
  static std::vector<Gamepad *> gamepads;  // TEMP: vector of gamepad objects

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

    // Allocate memory for gamepads
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
      gamepads.push_back(new Gamepad(i));

      if (glfwJoystickPresent(i))
      {
        gamepads[i]->EnableGamepad();
      }
    }
  }

  // Calls GLFW functions to check and store input
  void Update()
  {
    // Callback functions always called first; key states stored in "next state"

    // Read gamepad input
    UpdateGamepads();

    // Copy over current to previous key states
    UpdateKeyStates();
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

  // Upper left is (0,0)
  glm::vec2 ScreenToWorld(glm::vec2 cursor)
  {
	Camera *cam = Camera::GetActiveCamera();

	glm::vec2 screenPos = glm::vec2((cursor.x / (Settings::ScreenWidth() / 2)) - 1.0f, 1.0f - (cursor.y / (Settings::ScreenHeight() / 2)));

	glm::mat4 view = glm::lookAt(cam->m_Position, cam->m_Center, cam->m_Up);
	glm::mat4 proj = glm::ortho(-1.0f * cam->m_Zoom, 1.0f * cam->m_Zoom, -1.0f * cam->m_Zoom / cam->m_AspectRatio, 1.0f * cam->m_Zoom / cam->m_AspectRatio, cam->m_Near, cam->m_Far);
	glm::mat4 matrix = glm::inverse(proj * view);

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

    //if (IsHeldDown(key2))
    //{
    //  glm::vec2 coordinates = GetMousePos_World();
    //  printf("World coordinates: x = %f, y = %f\n", coordinates.x, coordinates.y);
    //}

  }

  // Callback for GLFW keyboard input detection
  // Stores keyboard input information to be set as Current State in next frame
  void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
  {
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
        //if (gamepads[i]->IsPressed(GamepadButton::B))
        //{
        //  std::cout << "UpdateGamepads: player " << i + 1
        //            << " pressed button B" << std::endl;
        //}

        //float axis = GamepadGetAxis(static_cast<PlayerNum>(i), GamepadAxis::LeftAnalog_X);

        //if (axis != 0)
        //{
        //  std::cout << "Player " << i << "X axis: " << axis << std::endl;
        //}
      }
    }
  }

  // Gets value for specific gamepad axis
  float GamepadGetAxis(PlayerNum player, GamepadAxis axis)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->GetGamepadAxis(axis);
    }
  }

  // Checks if gamepad button is pressed
  bool GamepadIsPressed(PlayerNum player, GamepadButton button)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->IsPressed(button);
    }
  }

  // Checks if gamepad button is held down
  bool GamepadIsHeldDown(PlayerNum player, GamepadButton button)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->IsHeldDown(button);
    }
  }

  // Checks if gamepad button is released
  bool GamepadIsReleased(PlayerNum player, GamepadButton button)
  {
    if (gamepads[player]->IsConnected())
    {
      return gamepads[player]->IsReleased(button);
    }
  }
}
