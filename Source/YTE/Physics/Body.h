/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-11-20
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Physics_Body_h
#define YTE_Physics_Body_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.h"
#include "YTE/Physics/ForwardDeclarations.h"

#include "YTE/Event/StandardEvents.h"

namespace YTE
{
  class Body : public Component
  {
  public:
    DeclareType(Body);

    Body(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~Body() override;

    void OnLogicUpdate(LogicUpdate *aEvent);

    void AddCollidedThisFrame(Composition *aComposition);

  protected:
    std::vector<Composition*> mCollidedThisFrame;
    std::vector<Composition*> mCollidedLastFrame;
  };
}
#endif
