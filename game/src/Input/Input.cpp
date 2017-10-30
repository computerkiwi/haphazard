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

namespace Input
{
  #define MAX_KEY_SIZE 350
  #define MAX_PLAYERS 4

  // Key to check, Last Key state, Current Key state
//  typedef std::map<int, std::pair<int, int>> TriggerMap;

  typedef std::vector<int> InputData;

  ////////// Static Variables //////////
  static GLFWwindow * inputWindow;         // Window to detect input from
//  static TriggerMap triggerMap;          // 0 to 9, A to Z, special characters, modifiers, numpad
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
    // Callback function always called first

    // Store states from callback

    // Probably change this to controls/commands
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
      if (gamepads[i]->IsConnected() == true)
      {
        gamepads[i]->Update();

        if (gamepads[i]->IsPressed(GamepadButton::RightTrigger))
        {
          printf("Button X is pressed\n");
        }
      }
    }

    if (IsPressed(Key::A))
    {
      std::cout << "A was pressed" << std::endl;
    }

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
    glm::vec2 origin;
    int world_x;
    int world_y;

    // Retrieve screen size
    glfwGetWindowSize(inputWindow, &world_x, &world_y);

    origin.x = world_x / 2;
    origin.y = world_y / 2;

    printf("GLFW world: x = %d, y = %d\n", world_x, world_y);
    printf("GLFW cursor pos: x = %f, y = %f\n", cursorPos.x, cursorPos.y);

 //   glm::


    return glm::vec2(cursor.x/ world_x, cursor.y/ world_y);
  }

  // Check if key is pressed; takes the key to check
  // Returns true if pressed
  bool IsPressed(int key)
  {
    //// Key pressed
    //if ((keyEvents[key].first == KeyState::Pressed) && (keyEvents[key].second == false))
    //{
    //  // Event handled
    //  keyEvents[key].second = true;

    //  return true;
    //}

    //if (triggerMap[key].first == KeyState::Released && triggerMap[key].second == KeyState::Pressed)
    //{
    //  return true;
    //}

    if ((prevState[key] == KeyState::Released) && (currState[key] == KeyState::Pressed))
    {
      return true;
    }

    // Key not pressed
    return false;
  }
  
  // Check if key is held down; takes the key to check
  // Returns true if held down
  bool IsHeldDown(int key)
  {
    //if ((keyEvents[key].first == KeyState::HeldDown) && (keyEvents[key].second == false))
    //{
    //  // Event handled
    //  keyEvents[key].second = true;

    //  return true;
    //}

    //if ((triggerMap[key].first == KeyState::Pressed) || (triggerMap[key].first == KeyState::HeldDown) &&
    //    (triggerMap[key].second == KeyState::Pressed) || (triggerMap[key].second == KeyState::HeldDown))
    //{
    //  return true;
    //}
    //

    if ((prevState[key] != KeyState::Released) && (currState[key] != KeyState::Released))
    {
      return true;
    }

    // Key not held down
    return false;
  }

  // Check if key has been released; takes key to check
  // Returns true if released
  bool IsReleased(int key)
  {
    //if ((keyEvents[key].first == KeyState::Released) && (keyEvents[key].second == false))
    //{
    //  // Event handled
    //  keyEvents[key].second = true;

    //  return true;
    //}

    //if (((triggerMap[key].first == KeyState::Pressed) || (triggerMap[key].first == KeyState::HeldDown)) && (triggerMap[key].second == KeyState::Released))
    //{
    //  return true;
    //}

    if ((prevState[key] == KeyState::Pressed) && (currState[key] == KeyState::Released))
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
  void InputDebug(int key)
  {
    char letter = static_cast<int>(key);

    if (IsPressed(key) == true)
    {
      printf("%c is pressed\n", letter);
    }
    else if (IsHeldDown(key) == true)
    {
      printf("%c is held down\n", letter);
    }
    else if (IsReleased(key) == true)
    {
      printf("%c is released\n", letter);
    }

    if (IsHeldDown(key))
    {
      glm::vec2 coordinates = GetMousePos_World();
      printf("World coordinates: x = %f, y = %f\n", coordinates.x, coordinates.y);
    }

  }

  // Callback for GLFW keyboard input detection
  // Stores keyboard input information
  void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
  {
    std::cout << "Callback called.\n";
    if (key)
    {
      switch (action)
      {
        // Key was pressed
        case GLFW_PRESS:
//          SetKeyState(key, KeyState::Pressed);
          nextState[key] = KeyState::Pressed;
          break;

        // Key was held down
        case GLFW_REPEAT:
//          SetKeyState(key, KeyState::HeldDown);
          nextState[key] = KeyState::HeldDown;
          break;

        // Key was released
        case GLFW_RELEASE:
//          SetKeyState(key, KeyState::Released);
          nextState[key] = KeyState::Released;
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
    if ((button >= 0) && (action == static_cast<int>(KeyState::Pressed)))
    {
      SetKeyState(button, KeyState::Pressed);
    }
  }

  // Detects when gamepad is connected
  void GamepadCallback(int joy, int event)
  {
    // Joystick connected
    if (event == GLFW_CONNECTED)
    {
      std::cout << "Gamepad connected" << std::endl;

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

  // Helper function for finding keys
  void SetKeyState(int key, int state)
  {
    // Set key state: first = previous, second = current; creates key is it doesn't exist
    //triggerMap[static_cast<Key>(key)].first = triggerMap[static_cast<Key>(key)].second;
    //triggerMap[static_cast<Key>(key)].second = state;

    //// State changed, add to key events
    //if (triggerMap[static_cast<Key>(key)].first != triggerMap[static_cast<Key>(key)].second)
    //{
    //  keyEvents[static_cast<Key>(key)].first = state;  // Change state
    //  keyEvents[static_cast<Key>(key)].second = false; // Event not handled
    //}

    currState[key] = state;
  }

  // Update previous state with current state
  void UpdateKeyStates()
  {
 //   TriggerMap::iterator it = triggerMap.begin();

 //   while (it != triggerMap.end())
 //   {
 //     //if (it->second.first != it->second.second)
 //     //{
 //     //  it->second.first = it->second.second;
 //     //}

 //     it->second.first = it->second.second;
 ////     it->second.second = KeyState::Released;

 //     ++it;
 //   }

    //InputData::iterator curr = currState.begin();
    //InputData::iterator prev = prevState.begin();
    //InputData::iterator next = nextState.begin();

    //while (curr != currState.end())
    //{
    //  prev->second = curr->second;

    //  next = nextState.find(curr->first);
    //  if (next != nextState.end())
    //  {
    //    curr->second = next->second;
    //  }

    //  ++curr;
    //  ++prev;
    //}

    for (int i = 0; i < MAX_KEY_SIZE; ++i)
    {
      prevState[i] = currState[i];
      currState[i] = nextState[i];
    }
  }
}
