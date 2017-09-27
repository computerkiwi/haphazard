#pragma once

typedef enum TRIGGER_STATE
{

  released,
  pressed,
  heldDown

}TRIGGER_STATE;

typedef enum KeyboardMap
{
  keyboard_UNKNOWN = -1,
  keyboard_SPACE = 32,
  keyboard_APOSTROPHE = 39, // '
  keyboard_COMMA = 44,      // ,
  keyboard_MINUS = 45,      // -
  keyboard_PERIOD = 46,     // .
  keyboard_SLASH = 47,      // /
  keyboard_0 = 48,
  keyboard_1 = 49,
  keyboard_2 = 50,
  keyboard_3 = 51,
  keyboard_4 = 52,
  keyboard_5 = 53,
  keyboard_6 = 54,
  keyboard_7 = 55,
  keyboard_8 = 56,
  keyboard_9 = 57,
  keyboard_SEMICOLON = 59,  // ;
  keyboard_EQUAL = 61,      // =
  keyboard_A = 65,
  keyboard_B = 66,
  keyboard_C = 67,
  keyboard_D = 68,
  keyboard_E = 69,
  keyboard_F = 70,
  keyboard_G = 71,
  keyboard_H = 72,
  keyboard_I = 73,
  keyboard_J = 74,
  keyboard_K = 75,
  keyboard_L = 76,
  keyboard_M = 77,
  keyboard_N = 78,
  keyboard_O = 79,
  keyboard_P = 80,
  keyboard_Q = 81,
  keyboard_R = 82,
  keyboard_S = 83,
  keyboard_T = 84,
  keyboard_U = 85,
  keyboard_V = 86,
  keyboard_W = 87,
  keyboard_X = 88,
  keyboard_Y = 89,
  keyboard_Z = 90,
  keyboard_BRACKET_LEFT = 91,
  keyboard_BACKSLASH = 92,
  keyboard_RIGHT_BRACKET = 93,
  keyboard_GRAVE_ACCENT = 96,
  keyboard__WORLD_1 = 161, // Non-US #1
  keyboard__WORLD_2 = 162, // Non-US #2
  keyboard_ESCAPE = 256,
  keyboard_ENTER = 257,
  keyboard_TAB = 258,
  keyboard_BACKSPACE = 259,
  keyboard_INSERT = 260,
  keyboard_DELETE = 261,
  keyboard_RIGHT = 262,
  keyboard_LEFT = 263,
  keyboard_DOWN = 264,
  keyboard_UP = 265,
  keyboard_PAGE_UP = 266,
  keyboard_PAGE_DOWN = 267,
  keyboard_HOME = 268,
  keyboard_END = 269,
  keyboard_CAPS_LOCK = 280,
  keyboard_SCROLL_OCK = 281,
  keyboard_NUM_LOCK = 282,
  keyboard_PRINT_SCREEN = 283,
  keyboard_PAUSE = 284,
  keyboard_F1 = 290,
  keyboard_F2 = 291,
  keyboard_F3 = 292,
  keyboard_F4 = 293,
  keyboard_F5 = 294,
  keyboard_F6 = 295,
  keyboard_F7 = 296,
  keyboard_F8 = 297,
  keyboard_F9 = 298,
  keyboard_F10 = 299,
  keyboard_F11 = 300,
  keyboard_F12 = 301,
  keyboard_F13 = 302,
  keyboard_F14 = 303,
  keyboard_F15 = 304,
  keyboard_NumPad_0 = 320,
  keyboard_NumPad_1 = 321,
  keyboard_NumPad_2 = 322,
  keyboard_NumPad_3 = 323,
  keyboard_NumPad_4 = 324,
  keyboard_NumPad_5 = 325,
  keyboard_NumPad_6 = 326,
  keyboard_NumPad_7 = 327,
  keyboard_NumPad_8 = 328,
  keyboard_NumPad_9 = 329,
  keyboard_NumPad_Decimal = 330,
  keyboard_NumPad_Divide = 331,
  keyboard_NumPad_Multiply = 332,
  keyboard_NumPad_Subtract = 333,
  keyboard_NumPad_Add = 334,
  keyboard_NumPad_Enter = 335,
  keyboard_NumPad_Equal = 336,
  keyboard_LEFT_SHIFT = 340,
  keyboard_LEFT_CONTROL = 341,
  keyboard_LEFT_ALT = 342,
  keyboard_RIGHT_SHIFT = 344,
  keyboard_RIGHT_CONTROL = 345,
  keyboard_RIGHT_ALT = 346

} KeyboardMap;

// Based on GLFW mouse map
typedef enum MouseMap
{

  mouse_1,
  mouse_2,
  mouse_3,
  mouse_4,
  mouse_5,
  mouse_6,
  mouse_7,
  mouse_8,
  mouse_last = mouse_8,
  mouse_BUTTON_LEFT = mouse_1,
  mouse_BUTTON_RIGHT = mouse_2,
  mouse_BUTTON_MIDDLE = mouse_3

} MouseMap;