/*******************************************************
FILE: Keybinding.h

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "KeyMap.h"
#include "Event.h"

// Holds the key and the command to execute for each player
class Keybinding
{
  private:
    PlayerID playerID_;
    KeybindingKeyboard * keyboard_;
    KeybindingGamepad * gamepad_;

  public:
    Keybinding(PlayerID playerID);
    ~Keybinding();

    void ResetAllKeybinding();
    void ResetKeyboardKeybinding();
    void ResetGamepadKeybinding();

    Key GetKeyboardKeybinding(Action action) const;
    GamepadButton GetGamepadKeybinding(Action action) const;
    GamepadAxis GetGamepadKeybindingAxis(Action action) const;

    void SetKeyboardJump(Key key);
    void SetKeyboardAttack(Key key);
    void SetKeyboardStack(Key key);
    void SetKeyboardMoveLeft(Key key);
    void SetKeyboardMoveRight(Key key);

    void SetGamepadJump(GamepadButton button);
    void SetGamepadAttack(GamepadButton button);
    void SetGamepadStack(GamepadButton button);
    void SetGamepadMoveAxis(GamepadAxis axis);
    void SetGamepadMoveButton(GamepadButton button, Action action); // For strange people who wanna move with butts
};

class KeybindingKeyboard
{
  private:
    PlayerID playerID_;
    Key jump_;
    Key attack_;
    Key stack_;
    Key moveLeft_;
    Key moveRight_;
    //Event * jumpEvent_;
    //Event * attackEvent_;
    //Event * stackEvent_;
    //Event * moveEvent_;

  public:
    KeybindingKeyboard(PlayerID playerID);
    ~KeybindingKeyboard();

    void ResetDefault();

    void SetJump(Key key);
    void SetAttack(Key key);
    void SetStack(Key key);
    void SetMoveLeft(Key key);
    void SetMoveRight(Key key);

    Key GetJump();
    Key GetAttack();
    Key GetStack();
    Key GetMoveLeft();
    Key GetMoveRight();
};

class KeybindingGamepad
{
  private:
    PlayerID playerID_;
    GamepadButton jump_;
    GamepadButton attack_;
    GamepadButton stack_;
    GamepadAxis move_;
    //Event * jumpEvent_;
    //Event * attackEvent_;
    //Event * stackEvent_;
    //Event * moveEvent_;

  public:
    KeybindingGamepad();
    ~KeybindingGamepad();

    void ResetDefault();

    void SetJump(GamepadButton button);
    void SetAttack(GamepadButton button);
    void SetStack(GamepadButton button);
    void SetMove(GamepadAxis axis);

    GamepadButton GetJump();
    GamepadButton GetAttack();
    GamepadButton GetStack();
    GamepadButton GetToss();
    GamepadAxis GetMoveAxisX();
    GamepadAxis GetMoveAxisY();
};