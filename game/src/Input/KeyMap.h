#pragma once

#define DEAD_ZONE_MIN -0.05
#define DEAD_ZONE_MAX  0.05

typedef enum KeyState : int
{
  Released = 0,
  Pressed  = 1,
  HeldDown = 2
} KeyState;

// Player number
typedef enum PlayerID : int
{
  Player1,
  Player2,
  Player3,
  Player4
} PlayerID;

typedef enum Action : int
{
  Jump,
  Attack,
  Stack,
  Toss,
  MoveLeft,
  MoveRight
} Action;

// All keyboard and mouse buttons
enum class Key : int
{
  // For fancy meese
  Mouse_1,
  Mouse_2,
  Mouse_3,
  Mouse_4,
  Mouse_5,
  Mouse_6,
  Mouse_7,
  Mouse_8,
  MouseButton_Last = Mouse_8,
  MouseButton_Left = Mouse_1,
  MouseButton_Right = Mouse_2,
  MouseButton_Middle = Mouse_3,

  Unknown = -1,
  Space = 32,
  Apostrophe = 39, // '
  Comma = 44,      // ,
  Minus = 45,      // -
  Period = 46,     // .
  Slash = 47,      // /

  Zero = 48,
  One = 49,
  Two = 50,
  Three = 51,
  Four = 52,
  Five = 53,
  Six = 54,
  Seven = 55,
  Eight = 56,
  Nine = 57,
  Semicolon = 59,  // ;
  Equal = 61,      // =

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

  BracketLeft = 91,
  Backslash = 92,
  BracketRight = 93,
  GraveAccent = 96,

  World_1 = 161, // Non-US #1
  World_2 = 162, // Non-US #2

  Escape = 256,
  Enter = 257,
  Tab = 258,
  Backspace = 259,
  Insert = 260,
  Delete = 261,

  Right = 262,
  Left = 263,
  Down = 264,
  Up = 265,

  PageUp = 266,
  PageDown = 267,
  Home = 268,
  End = 269,
  CapsLock = 280,
  ScrollLock = 281,
  NumLock = 282,
  PrintScreen = 283,
  Pause = 284,

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

  Numpad_0 = 320,
  Numpad_1 = 321,
  Numpad_2 = 322,
  Numpad_3 = 323,
  Numpad_4 = 324,
  Numpad_5 = 325,
  Numpad_6 = 326,
  Numpad_7 = 327,
  Numpad_8 = 328,
  Numpad_9 = 329,
  NumpadDecimal = 330,
  NumpadDivide = 331,
  NumpadMultiply = 332,
  NumpadSubtract = 333,
  NumpadAdd = 334,
  NumpadEnter = 335,
  NumpadEqual = 336,

  LeftShift = 340,
  LeftControl = 341,
  LeftAlt = 342,
  RightShift = 344,
  RightControl = 345,
  RightAlt = 346

};

enum class GamepadButton : int
{
  A,
  B,
  X,
  Y,
  LeftTrigger,
  RightTrigger,
  Back,
  Start,
  Guide,          // Which button is this?
  LeftThumb,
  RightThumb,
  Up,
  Right,
  Down,
  Left
};

enum class GamepadAxis : int
{
  LeftAnalog_X,
  LeftAnalog_Y,
  RightAnalog_X,
  RightAnalog_Y,
  LeftTrigger,
  RightTrigger,

  LeftAnalog,  // Both left XY axes
  RightAnalog, // Both right XY axes

  Horizontal = LeftAnalog_X,
  Vertical = LeftAnalog_Y,
};