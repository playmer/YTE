/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   11/28/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Utilities_Reactive_h
#define YTE_Utilities_Reactive_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/MenuCollider.h"

namespace YTE
{
  DeclareEvent(MouseEnter);
  DeclareEvent(MouseExit);
  class MouseEnter : public Event
  {
  public:
    DeclareType(MouseEnter);

    String Name;
  };

  class MouseExit : public Event
  {
  public:
    DeclareType(MouseExit);

    String Name;
  };

  class Reactive : public Component
  {
  public:
    DeclareType(Reactive);

    Reactive(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void OnLogicUpdate(LogicUpdate *aEvent);
    void OnCollisionStarted(CollisionStarted *aEvent);
    void OnCollisionEnded(CollisionEnded *aEvent);
    void OnMousePress(MouseButtonEvent *aEvent);
    void OnMouseRelease(MouseButtonEvent *aEvent);

  private:
    MenuCollider* mMenuCollider;
    bool mIsMouseEntered;
  };
}

#endif
