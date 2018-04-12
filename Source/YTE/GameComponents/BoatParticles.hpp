/******************************************************************************/
/*!
\file   BoatParticles.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-03-22
\brief
    This component controls the particle emitters parented to the boat.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_BoatParticles_hpp
#define YTE_Gameplay_BoatParticles_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class BoatController;
  class ParticleEmitter;

  class BoatParticles : public Component
  {
  public:
    YTEDeclareType(BoatParticles);
    BoatParticles(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate *aEvent);

    float GetVarianceScalar() const;
    void SetVarianceScalar(float aScalar);

    float GetFrontWeight() const;
    void SetFrontWeight(float aWeight);

    float GetSideWeight() const;
    void SetSideWeight(float aWeight);

  private:
    Transform *mTransform;
    Orientation *mOrientation;
    RigidBody *mRigidBody;

    BoatController *mBoatController;

    ParticleEmitter *mFrontRightEmitter;
    ParticleEmitter *mFrontLeftEmitter;

    ParticleEmitter *mFrontRightShooter;
    ParticleEmitter *mFrontLeftShooter;
    
    ParticleEmitter *mSideRightEmitter;
    ParticleEmitter *mSideLeftEmitter;

    ParticleEmitter *mFrontEmitter;
    ParticleEmitter *mBackEmitter;

    float mVarianceScalar;
    float mFrontWeight;
    float mSideWeight;
  };
} 

#endif
