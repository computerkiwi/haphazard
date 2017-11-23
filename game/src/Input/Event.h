/*******************************************************
FILE: Event.h

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "KeyMap.h"

// Virtual class for commands
class Event
{
  public:
    virtual ~Event() {}
    virtual void Execute() = 0; // Must be implemented by derived class
};

class JumpEvent
{
  private:


  public:

};

class StackEvent
{

};

class AttackEvent
{

};

class TossEvent
{

};