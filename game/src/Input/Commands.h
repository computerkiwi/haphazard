/*******************************************************
FILE: Commands.h

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "KeyMap.h"

// Virtual class for commands
class Command
{
  public:
    virtual ~Command() {}
    virtual void Execute() = 0; // Must be implemented by derived class
};

class CommandJump
{
  public:

};

class CommandStack
{

};

class CommandAttack
{

};
