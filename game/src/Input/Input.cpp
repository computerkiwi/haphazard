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

  void Init(GLFWwindow * window)
  {
    // Set default controls

    // Map keys

    inputWindow = window;

  }

  void Update()
  {
      // Action: press, repeat, release
      // Input mode (GLFW set input mode): sticky keys (makes sure a key is polled, event stays until handled)

    glfwSetKeyCallback(inputWindow, KeyCallback);
    glfwSetCursorPosCallback(inputWindow, CursorCallback);
    glfwSetMouseButtonCallback(inputWindow, MouseButtonCallback);

    // Not sure what this means: On platforms that provide it, the full sub - pixel cursor position is passed on.


  }

  void Exit()
  {
  }

  void SetControls()
  {

  }

  // Debug
  void KeyCheck_Debug(int key)
  {
    if (triggerMap[key] == pressed)
    {
      std::cout << "KeyCheck: Key pressed" << std::endl;
    }
    else if (triggerMap[key] == heldDown)
    {
      std::cout << "KeyCheck: Key held down" << std::endl;
    }
  }


  // Array slot of each possible key that stores current state; modify state whenever pressed/released
  // Function for checking which key is pressed
  // Mouse position, key presses
  // Get axis from Unity (tutorial for input scheme)
  void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
  {
    if (key)
    {
      switch (action)
      {
        // Key was pressed
        case GLFW_PRESS:
          triggerMap[key] = pressed;

          std::cout << "KEY " << key << " pressed" << std::endl;
          break;

        // Key was held down
        case GLFW_REPEAT:
          triggerMap[key] = heldDown;

          std::cout << "KEY " << key << " held down" << std::endl;
          break;

        // Key was released
        case GLFW_RELEASE:

          triggerMap[key] = released;

          std::cout << "KEY " << key << " released" << std::endl;
      }
    }
  }

  void CursorCallback(GLFWwindow * window, double xpos, double ypos)
  {
    //double x_pos;
    //double y_pos;

    //glfwGetCursorPos(inputWindow, &x_pos, &y_pos);

    cursorPos.x = (float)xpos;
    cursorPos.y = (float)ypos;

    std::cout << "CURSOR POSITION: x = " << cursorPos.x << ", y = " << cursorPos.y << std::endl;
  }

  void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod)
  {
    if ((button >= 0) && (action == pressed))
    {
      triggerMap[button] = pressed;

      std::cout << "MOUSE BUTTON " << button << " was pressed" << std::endl;
    }
  }
}