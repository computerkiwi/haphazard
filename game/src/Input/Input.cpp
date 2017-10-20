/*******************************************************
FILE: Input.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Input.h"
#include <iostream>

#include <imgui.h>
#include "Imgui\imgui-setup.h"

// Command-based: jump, left, right, attack
// Keep binding in mind


/*Gamepad::Gamepad() : moveLeft_(GLFW_KEY_A), moveRight_(GLFW_KEY_D)
                     , attack_(GLFW_KEY_RIGHT_ALT), jump_(GLFW_KEY_SPACE)
{
}

Gamepad const & Gamepad::GetConfig() const
{
  return *this;
} 

Input::Input(GLFWwindow * window) : window_(window) //, keyMap_()
{
} */

namespace Input
{

  // Key to check, Last Key state, Current Key state
  typedef std::map<Key, std::pair<KeyState, KeyState>> TriggerMap;

  ////////// Static Variables //////////
  static GLFWwindow * inputWindow;                           // Window to detect input from
  static TriggerMap triggerMap;                              // 0 to 9, A to Z, special characters, modifiers, numpad
//  static std::vector<std::pair<Key, KeyState>> keyEvents;  // Key events
  static std::map<Key, std::pair<KeyState, bool>> keyEvents; // Key to check, key's state, whether or not even was handled
  static glm::vec2 cursorPos;                                // x, y cursor positions; top-left is origin
  static std::vector<Gamepad *> gamepads;                    // TEMP: vector of gamepad objects

//  static Gamepad * player1;

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
    // Input mode (GLFW set input mode): sticky Key (makes sure a key is polled, event stays until handled)

    // Callbacks for GLFW
    glfwSetKeyCallback(inputWindow, KeyCallback);
    glfwSetCursorPosCallback(inputWindow, CursorCallback);
    glfwSetMouseButtonCallback(inputWindow, MouseButtonCallback);

    //glfwPollEvents();

    // Detects gamepads
    glfwSetJoystickCallback(GamepadCallback);
    
    // Gamepads connected
    if (gamepads.size() > 0)
    {
      for (int i = 0; i < gamepads.size(); ++i)
      {
        gamepads[0]->GamepadDebug();
      }
    }

//    ResetKeyStates();
    /*
    for (size_t i = 0; i < gamepads.size(); ++i)
    {
      if (gamepads[i] != NULL)
      {
        int axes; // ???? Why do I need the address whyyy

        glfwGetJoystickAxes(gamepads[i]->GetGamepadID(), &axes);

        gamepads[i]->SetAxes(axes);


        std::cout << "ID: " << gamepads[i]->GetGamepadID() << std::endl;
        std::cout << "AXES: " << gamepads[i]->GetGamepadAxis() << std::endl;
      }
    } */

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
  glm::vec2 ScreenToWorld(glm::vec2 screen)
  {
    int world_x;
    int world_y;

    glfwGetWindowSize(inputWindow, &world_x, &world_y);
    printf("GLFW world: x = %d, y = %d\n", world_x, world_y);
    printf("GLFW cursor pos: x = %f, y = %f\n", cursorPos.x, cursorPos.y);
    return glm::vec2(screen.x/ world_x, screen.y/ world_y);
  }

  // Check if key is pressed; takes the key to check
  // Returns true if pressed
  bool IsPressed(Key key)
  {
    //// Key pressed
    //if ((triggerMap[key].first == KeyState::Released) && (triggerMap[key].second == KeyState::Pressed))
    //{
    //  // Reset key state
    //  triggerMap[key].second = KeyState::Released;

    //  return true;
    //}

    // Key pressed
    if ((keyEvents[key].first == KeyState::Pressed) && (keyEvents[key].second == false))
    {
      // Event handled
      keyEvents[key].second = true;

      return true;
    }

    // Key not pressed
    return false;
  }
  
  // Check if key is held down; takes the key to check
  // Returns true if held down
  bool IsHeldDown(Key key)
  {
    //// Key was pressed or held down, and not currently released
    //if (((triggerMap[key].first == KeyState::Pressed) || (triggerMap[key].first == KeyState::HeldDown)) &&
    //     (triggerMap[key].second != KeyState::Released))
    //{

    //  // Event handled
    //  keyEvents[key].second = true;

    //  return true;
    //}

    if ((keyEvents[key].first == KeyState::HeldDown) && (keyEvents[key].second == false))
    {
      // Event handled
      keyEvents[key].second = true;

      return true;
    }
    
    // Key not held down
    return false;
  }

  // Check if key has been released; takes key to check
  // Returns true if released
  bool IsReleased(Key key)
  {
    //// Key was pressed or held down, and now released
    //if (((triggerMap[key].first == KeyState::Pressed) || (triggerMap[key].first == KeyState::HeldDown)) &&
    //     (triggerMap[key].second == KeyState::Released))
    //{
    //  // Event handled
    //  keyEvents[key].second = true;

    //  return true;
    //}

    if ((keyEvents[key].first == KeyState::Released) && (keyEvents[key].second == false))
    {
      // Event handled
      keyEvents[key].second = true;

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
  void InputDebug(Key key)
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
    else if (Input::IsReleased(key) == true)
    {
      printf("%c is released\n", letter);
    }

    if (Input::IsHeldDown(key))
    {
      glm::vec2 coordinates = GetMousePos_World();
      printf("World coordinates: x = %f, y = %f\n", coordinates.x, coordinates.y);
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
          SetKeyState(key, KeyState::Pressed);
          break;

        // Key was held down
        case GLFW_REPEAT:
          SetKeyState(key, KeyState::HeldDown);
          break;

        // Key was released
        case GLFW_RELEASE:
          SetKeyState(key, KeyState::Released);
 //         triggerMap[static_cast<Key>(key)].first = KeyState::Released;
      }

    // Noah does a thing
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
  }

  // Stores mouse button input information
  void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod)
  {
    // Button was pressed
    if ((button >= 0) && (action == static_cast<int>(KeyState::Pressed)))
    {
      SetKeyState(button, KeyState::Pressed);

 //     std::cout << "MOUSE BUTTON " << button << " was pressed" << std::endl;
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

  // Helper function for finding keys
  void SetKeyState(int key, KeyState state)
  {
    // Set key state: first = previous, second = current; creates key is it doesn't exist
    triggerMap[static_cast<Key>(key)].first = triggerMap[static_cast<Key>(key)].second;
    triggerMap[static_cast<Key>(key)].second = state;

    // State changed, add to key events
    if (triggerMap[static_cast<Key>(key)].first != triggerMap[static_cast<Key>(key)].second)
    {
//      keyEvents.push_back(std::make_pair(static_cast<Key>(key), state));

      keyEvents[static_cast<Key>(key)].first = state;
      keyEvents[static_cast<Key>(key)].second = false;

//      printf("State changed %d to %d\n", key, state);
    }
  }

  // Reset all key states to released
  void ResetKeyStates()
  {
    TriggerMap::iterator it = triggerMap.begin();

    while (it != triggerMap.end())
    {
      it->second.first = KeyState::Released;
      it->second.second = KeyState::Released;

      ++it;
    }
  }
}
