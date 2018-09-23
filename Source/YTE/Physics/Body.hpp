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

#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class Body : public Component
  {
  public:
    YTEDeclareType(Body);

    YTE_Shared Body(Composition *aOwner, Space *aSpace);

    YTE_Shared ~Body() override;

    YTE_Shared void OnLogicUpdate(LogicUpdate *aEvent);

    YTE_Shared void AddCollidedThisFrame(Composition *aComposition);

  protected:
    std::vector<Composition*> mCollidedThisFrame;
    std::vector<Composition*> mCollidedLastFrame;
  };
}
#endif
