/******************************************************************************/
/*!
\file   OceanCreatureBehavior.hpp
\author Nicholas Ammann
\par    email : nicholas.ammann\@digipen.edu
\date   2018 - 04 - 15

All content(c) 2016 DigiPen(USA) Corporation, all rights reserved.
* /
/******************************************************************************/
#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Physics/Transform.hpp"

namespace YTE
{

  class OceanCreatureBehavior : public Component
  {
  public:
    YTEDeclareType(OceanCreatureBehavior);
    OceanCreatureBehavior(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void Update(LogicUpdate *aEvent);

    bool GetFlipRotation() const;
    void SetFlipRotation(bool aFlip);

  private:
    Transform *mParentTransform;
    Transform *mBoatTransform;

    bool mFlipRotation;

    bool mStartJump;
    bool mIsJumping;

    double mTimer;

  };
} //end yte
