#pragma once

// State the key is in: pressed, released, held down
enum class KeyState : int
{

  released,
  pressed,
  heldDown

}; 

// Pressable keys and buttons (keyboard and mouse)
enum class Key : int
{
  MOUSE_1,
  MOUSE_2,
  MOUSE_3,
  MOUSE_4,
  MOUSE_5,
  MOUSE_6,
  MOUSE_7,
  MOUSE_8,
  MOUSE_LAST = MOUSE_8,
  MOUSE_BUTTON_LEFT = MOUSE_1,
  MOUSE_BUTTON_RIGHT = MOUSE_2,
  MOUSE_BUTTON_MIDDLE = MOUSE_3,

  UNKNOWN = -1,
  SPACE = 32,
  APOSTROPHE = 39, // '
  COMMA = 44,      // ,
  MINUS = 45,      // -
  PERIOD = 46,     // .
  SLASH = 47,      // /

  ZERO = 48,
  ONE = 49,
  TWO = 50,
  THREE = 51,
  FOUR = 52,
  FIVE = 53,
  SIX = 54,
  SEVEN = 55,
  EIGHT = 56,
  NINE = 57,
  SEMICOLON = 59,  // ;
  EQUAL = 61,      // =

  A = 65,
  B = 66,
  C = 67,
  D = 68,
  E = 69,
  F = 70,
  G = 71,
  H = 72,
  I = 73,
  J = 74,
  K = 75,
  L = 76,
  M = 77,
  N = 78,
  O = 79,
  P = 80,
  Q = 81,
  R = 82,
  S = 83,
  T = 84,
  U = 85,
  V = 86,
  W = 87,
  X = 88,
  Y = 89,
  Z = 90,

  BRACKET_LEFT = 91,
  BACKSLASH = 92,
  RIGHT_BRACKET = 93,
  GRAVE_ACCENT = 96,

  WORLD_1 = 161, // Non-US #1
  WORLD_2 = 162, // Non-US #2

  ESCAPE = 256,
  ENTER = 257,
  TAB = 258,
  BACKSPACE = 259,
  INSERT = 260,
  DELETE = 261,

  RIGHT = 262,
  LEFT = 263,
  DOWN = 264,
  UP = 265,

  PAGE_UP = 266,
  PAGE_DOWN = 267,
  HOME = 268,
  END = 269,
  CAPS_LOCK = 280,
  SCROLL_OCK = 281,
  NUM_LOCK = 282,
  PRINT_SCREEN = 283,
  PAUSE = 284,

  F1 = 290,
  F2 = 291,
  F3 = 292,
  F4 = 293,
  F5 = 294,
  F6 = 295,
  F7 = 296,
  F8 = 297,
  F9 = 298,
  F10 = 299,
  F11 = 300,
  F12 = 301,
  F13 = 302,
  F14 = 303,
  F15 = 304,

  NUMPAD_0 = 320,
  NUMPAD_1 = 321,
  NUMPAD_2 = 322,
  NUMPAD_3 = 323,
  NUMPAD_4 = 324,
  NUMPAD_5 = 325,
  NUMPAD_6 = 326,
  NUMPAD_7 = 327,
  NUMPAD_8 = 328,
  NUMPAD_9 = 329,
  NUMPAD_DECIMAL = 330,
  NUMPAD_DIVIDE = 331,
  NUMPAD_MULTIPLY = 332,
  NUMPAD_SUBTRACT = 333,
  NUMPAD_ADD = 334,
  NUMPAD_ENTER = 335,
  NUMPAD_EQUAL = 336,

  LEFT_SHIFT = 340,
  LEFT_CONTROL = 341,
  LEFT_ALT = 342,
  RIGHT_SHIFT = 344,
  RIGHT_CONTROL = 345,
  RIGHT_ALT = 346

};

// Gamepad buttons: A, B, X, Y, top triggers, directional buttons
enum class GamepadButton : int
{
   A,
   B,
   X,
   Y,
   LEFT_TRIGGER,
   RIGHT_TRIGGER,
   BACK,
   START,
   LEFT_THUMB,
   RIGHT_THUMB,
   UP,
   RIGHT,
   DOWN,
   LEFT

};

// Gamepad axes: analog sticks, bottom triggers (sensitivity)
enum class GamepadAxis : int
{
  LEFT_ANALOG_X,
  LEFT_ANALOG_Y,
  RIGHT_ANALOG_X,
  RIGHT_ANALOG_Y,
  LEFT_TRIGGER,
  RIGHT_TRIGGER,

  LEFT_ANALOG,  // Both left XY axes
  RIGHT_ANALOG, // Both right XY axes
};