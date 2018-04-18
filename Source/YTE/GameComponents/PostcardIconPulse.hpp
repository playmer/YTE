/******************************************************************************/
/*!
\file   PostcardIconPulse.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-03-22
\brief
    This component controls the particle emitters parented to the boat.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_PostcardPulse_hpp
#define YTE_Gameplay_PostcardPulse_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class PostcardIconPulse : public Component
  {
  public:
    YTEDeclareType(PostcardIconPulse);
    PostcardIconPulse(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate *aEvent);

    void SetPulsing(bool aIsPulsing);

  private:
    Transform *mTransform;

    glm::vec3 mInitialScale;

    double mTimer;

    bool mIsPulsing;

  };
} 

#endif
