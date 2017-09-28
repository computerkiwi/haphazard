/*******************************************************
FILE: Input.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Input.h"
#include <iostream>

// Command-based: jump, left, right, attack
// Keep binding in mind


/*Controls::Controls() : moveLeft_(GLFW_KEY_A), moveRight_(GLFW_KEY_D)
                     , attack_(GLFW_KEY_RIGHT_ALT), jump_(GLFW_KEY_SPACE)
{
}

Controls const & Controls::GetConfig() const
{
  return *this;
} 

Input::Input(GLFWwindow * window) : window_(window) //, keyMap_()
{
} */

namespace Input
{

  typedef std::map<KeyboardMap, KeyState> TriggerMap;

  ////////// Static Variables //////////
  static GLFWwindow * inputWindow;    // Window to detect input from
  static TriggerMap triggerMap;       // 0 to 9, A to Z, special characters, modifiers, numpad
  static glm::vec2 cursorPos;         // x, y cursor positions; top-left is origin

  static ControllerMap player1;
//  static ControllerMap player2;
//  static ControllerMap player3;
//  static ControllerMap player4;

//  static Controls * player1;

  // Initializes input window for detection
  void Init(GLFWwindow * window)
  {
    // Set default controls

    if (window)
    {
      inputWindow = window;
 //     player1Controls
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
      // Input mode (GLFW set input mode): sticky keys (makes sure a key is polled, event stays until handled)

    glfwSetKeyCallback(inputWindow, KeyCallback);
    glfwSetCursorPosCallback(inputWindow, CursorCallback);
    glfwSetMouseButtonCallback(inputWindow, MouseButtonCallback);

    // Detects controller
    glfwSetJoystickCallback(ControllerCallback);



  }

  // Check if key is pressed; takes the key to check
  // Returns true if pressed
  bool IsPressed(KeyboardMap key)
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
  bool IsHeldDown(KeyboardMap key)
  {
    // Key held down
    if (triggerMap[key] == KeyState::heldDown)
    {
      return true;
    }
    
    // Key not held down
    return false;
  }

  void Exit()
  {
  }

  void SetControls()
  {

  }

  // Debug
  void Input_Debug(KeyboardMap key)
  {
    if (Input::IsPressed(key) == true)
    {
      printf("A is pressed\n");
    }
    else if (Input::IsHeldDown(key) == true)
    {
      printf("A is held down\n");
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
          triggerMap[static_cast<KeyboardMap>(key)] = KeyState::pressed;

      //    std::cout << "KEY " << key << " pressed" << std::endl;
          break;

        // Key was held down
        case GLFW_REPEAT:
          triggerMap[static_cast<KeyboardMap>(key)] = KeyState::heldDown;

       //   std::cout << "KEY " << key << " held down" << std::endl;
          break;

        // Key was released
        case GLFW_RELEASE:

          triggerMap[static_cast<KeyboardMap>(key)] = KeyState::released;

          std::cout << "KEY " << key << " released" << std::endl;
      }
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
      triggerMap[static_cast<KeyboardMap>(button)] = KeyState::pressed;

      std::cout << "MOUSE BUTTON " << button << " was pressed" << std::endl;
    }
  }

  // Detects when controller is connected
  void ControllerCallback(int joy, int event)
  {
    // Joystick connected
    if (event == GLFW_CONNECTED)
    {
      std::cout << "Controller connected" << std::endl;

      // Match controller ID to player
      // Set controls

    }
    // Joystick disconnected
    else if (event == GLFW_DISCONNECTED)
    {
      std::cout << "Controller disconnected" << std::endl;
    }
  }
}
