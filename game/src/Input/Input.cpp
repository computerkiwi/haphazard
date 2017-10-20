/*******************************************************
FILE: Input.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Input.h"
#include "graphics\Screen.h"
#include <iostream>

#include <imgui.h>
#include "Imgui\imgui-setup.h"

// Command-based: jump, left, right, attack
// Keep binding in mind


namespace Input
{
  ////////// Defined Types //////////
  // Key: key that was pressed; Keystate 1: previous state; Keystate 2: current state
  typedef std::map<Key, std::pair<KeyState, KeyState>> TriggerMap;

  ////////// Static Variables //////////
  static GLFWwindow * inputWindow;           // Window to detect input from
  static TriggerMap triggerMap;              // 0 to 9, A to Z, special characters, modifiers, numpad
  static glm::vec2 cursorPos;                // x, y cursor positions; top-left is origin
  static std::vector<Gamepad *> gamepads;    // TEMP: vector of gamepad objects

  ////////// Static Callback Functions //////////
  static void KeyCallback(GLFWwindow * window, int key, int scancode, int actions, int mods);
  static void CursorCallback(GLFWwindow * window, double xpos, double ypos);
  static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod);
  static void GamepadCallback(int joy, int event);

  // Initializes input window for detection
  void Init(GLFWwindow * window)
  {
    // Set default Gamepad

    if (window)
    {
      inputWindow = window;
 //     player1Gamepad
    }
    else
    {
      std::cout << "No input window set" << std::endl;
    }

  }

  // Calls GLFW functions to check and store input
  void Update()
  {
    // Action: press, repeat, release
    // Input mode (GLFW set input mode): sticky Key (makes sure a key is polled, event stays until handled)

    glfwSetKeyCallback(inputWindow, KeyCallback);
    glfwSetCursorPosCallback(inputWindow, CursorCallback);
    glfwSetMouseButtonCallback(inputWindow, MouseButtonCallback);

    // Detects gamepads
    glfwSetJoystickCallback(GamepadCallback);
    
    // Gamepads connected
    if (gamepads.size() > 0)
    {
      gamepads[0]->GamepadDebug();
    }

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

  // Check if key is pressed; takes the key to check
  // Returns true if pressed
  bool IsPressed(Key key)
  {
    // Key pressed
    if (triggerMap[key] == KeyState::pressed)
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
    // Key held down
    if (triggerMap[key] == KeyState::heldDown)
    {
      return true;
    }
    
    // Key not held down
    return false;
  }

  // Return mouse position
  glm::vec2 GetMousePos()
  {
    return cursorPos;
  }


  // Merp
  glm::vec2 ScreenToWorld(glm::vec2 screen)
  {
    return cursorPos - screen;
  }

  void SetGamepad()
  {

  }

  // Debug
  void Input_Debug(Key key)
  {
    char letter = static_cast<int>(key);

    if (Input::IsPressed(key) == true)
    {
      printf("%c is pressed\n", letter);
    }
    else if (Input::IsHeldDown(key) == true)
    {
      printf("%c is held down\n", letter);
    }
  }


  // Array slot of each possible key that stores current state; modify state whenever pressed/released
  // Function for checking which key is pressed
  // Mouse position, key presses
  // Get axis from Unity (tutorial for input scheme)

  // Callback for GLFW keyboard input detection
  // Stores keyboard input information
  void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
  {
    if (key)
    {
      switch (action)
      {
        // Key was pressed
        case GLFW_PRESS:
          triggerMap[static_cast<Key>(key)].second = KeyState::heldDown;

      //    std::cout << "KEY " << key << " pressed" << std::endl;
          break;

        // Key was held down
        case GLFW_REPEAT:
          triggerMap[static_cast<Key>(key)].second = KeyState::heldDown;

       //   std::cout << "KEY " << key << " held down" << std::endl;
          break;

        // Key was released
        case GLFW_RELEASE:

          triggerMap[static_cast<Key>(key)].second = KeyState::released;

          std::cout << "KEY " << key << " released" << std::endl;
      }
	  ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    }
  }

  // Stores mouse position
  void CursorCallback(GLFWwindow * window, double xpos, double ypos)
  {
    //double x_pos;
    //double y_pos;

    //glfwGetCursorPos(inputWindow, &x_pos, &y_pos);

    cursorPos.x = (float)xpos;
    cursorPos.y = (float)ypos;

    std::cout << "CURSOR POSITION: x = " << cursorPos.x << ", y = " << cursorPos.y << std::endl;
  }

  // Stores mouse button input information
  void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod)
  {
    if ((button >= 0) && (action == static_cast<int>(KeyState::pressed)))
    {
      triggerMap[static_cast<Key>(button)] = KeyState::pressed;

      std::cout << "MOUSE BUTTON " << button << " was pressed" << std::endl;
    }
  }

  // Detects when gamepad is connected
  void GamepadCallback(int joy, int event)
  {
    // Joystick connected
    if (event == GLFW_CONNECTED)
    {
      std::cout << "Gamepad connected" << std::endl;

      // Add gamepad ID to player list
      gamepads.push_back(new Gamepad(joy));

      // Set Gamepad

    }
    // Joystick disconnected
    else if (event == GLFW_DISCONNECTED)
    {
      std::cout << "Gamepad disconnected" << std::endl;
    }
  }


}
