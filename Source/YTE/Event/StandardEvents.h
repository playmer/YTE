/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_Event_StandardEvents_h
#define YTE_Event_StandardEvents_h

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"

#include "YTE/Event/Events.h"

namespace YTE
{
  namespace Events
  {
    // Core
    extern const std::string LogicUpdate;
    extern const std::string FrameUpdate;
    extern const std::string BeginDebugDrawUpdate;
    extern const std::string DebugDrawUpdate;
    extern const std::string EndDebugDrawUpdate;
    extern const std::string DeletionUpdate;
    extern const std::string BoundTypeChanged;

    //Physics

  }

  class LogicUpdate : public Event
  {
  public:
    DeclareType(LogicUpdate);

    float Dt;
  };

  class BoundTypeChanged : public Event
  {
  public:
    DeclareType(BoundTypeChanged);

    BoundType *aOldType;
    BoundType *aNewType;
  };
   




} // End yte

#endif
