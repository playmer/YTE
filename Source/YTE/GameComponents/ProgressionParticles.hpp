/******************************************************************************/
/*!
\file   ProgressionParticles.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-03-22
\brief

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_ProgressionParticles_hpp
#define YTE_Gameplay_ProgressionParticles_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class ParticleEmitter;
  class PostcardUpdate;

  YTEDeclareEvent(ProgressionHappened);
  class ProgressionHappened : public Event
  {
  public:
    YTEDeclareType(ProgressionHappened);
  };

  class ProgressionParticles : public Component
  {
  public:
    YTEDeclareType(ProgressionParticles);
    ProgressionParticles(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate *aEvent);

    double GetPoofTime() const;
    void SetPoofTime(double aTime);

    int GetPoofCount() const;
    void SetPoofCount(int aCount);
    
    float GetPoofEmitRate() const;
    void SetPoofEmitRate(float aTime);

    void OnProgressionHappened(ProgressionHappened *aEvent);

  private:
    ParticleEmitter *mProgressionEmitter;
    RigidBody *mRigidBody;

    double mTimer;

    // make a particles poof :)
    bool mMakePoof;

    double mPoofTime;
    int mPoofCount;
    float mPoofEmitRate;

  };
} 

#endif
