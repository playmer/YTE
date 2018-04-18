/******************************************************************************/
/*!
\file   ProgressionParticles.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-03-22

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/ProgressionParticles.hpp"
#include "YTE/GameComponents/BoatController.hpp"
#include "YTE/GameComponents/HudController.hpp"

#include "YTE/Graphics/ParticleEmitter.hpp"

#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Orientation.hpp"

namespace YTE
{
  YTEDefineEvent(ProgressionHappened);

  YTEDefineType(ProgressionHappened)
  {
    YTERegisterType(ProgressionHappened);
  }

  YTEDefineType(ProgressionParticles)
  {
    YTERegisterType(ProgressionParticles);

    YTEBindProperty(&ProgressionParticles::GetPoofTime, &ProgressionParticles::SetPoofTime, "Poof Time")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ProgressionParticles::GetPoofCount, &ProgressionParticles::SetPoofCount, "Poof Count")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ProgressionParticles::GetPoofEmitRate, &ProgressionParticles::SetPoofEmitRate, "Poof Emit Rate")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  ProgressionParticles::ProgressionParticles(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mProgressionEmitter(nullptr)
    , mTimer(0.0)
    , mPoofTime(2.0)
    , mMakePoof(false)
    , mPoofCount(0)
    , mPoofEmitRate(1000.0f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ProgressionParticles::Initialize()
  {
    if (Composition *emitterObj = mOwner->FindFirstCompositionByName("ProgressionEmitter"))
    {
       mProgressionEmitter = emitterObj->GetComponent<ParticleEmitter>();
    }

    mRigidBody = mOwner->GetComponent<RigidBody>();

    mSpace->YTERegister(Events::LogicUpdate, this, &ProgressionParticles::Update);

    mSpace->YTERegister(Events::ProgressionHappened, this, &ProgressionParticles::OnProgressionHappened);

    mPoofCount = 0;
    mPoofEmitRate = 1000.0f;
    mProgressionEmitter->SetEmitCount(mPoofCount);
    mProgressionEmitter->SetEmitRate(mPoofEmitRate);
  }

  void ProgressionParticles::Update(LogicUpdate *aEvent)
  {
    if (mProgressionEmitter)
    {
      if (mMakePoof)
      {
        if (mTimer > mPoofTime)
        {
          mTimer = 0.0;
          mProgressionEmitter->SetEmitCount(0);
          mProgressionEmitter->SetEmitRate(10000.0f);
          mMakePoof = false;
        }
        else
        {
          mProgressionEmitter->SetInitVelocity(mRigidBody->GetVelocity());
          mTimer += aEvent->Dt;
        }
      }
    }
  }

  double ProgressionParticles::GetPoofTime() const
  {
    return mPoofTime;
  }

  void ProgressionParticles::SetPoofTime(double aTime)
  {
    mPoofTime = aTime;
  }

  int ProgressionParticles::GetPoofCount() const
  {
    return mPoofCount;
  }

  void ProgressionParticles::SetPoofCount(int aCount)
  {
    mPoofCount = aCount;
  }

  float ProgressionParticles::GetPoofEmitRate() const
  {
    return mPoofEmitRate;
  }

  void ProgressionParticles::SetPoofEmitRate(float aTime)
  {
    mPoofEmitRate = aTime;
  }

  void ProgressionParticles::OnProgressionHappened(ProgressionHappened * aEvent)
  {
    YTEUnusedArgument(aEvent);
    mMakePoof = true;
    mProgressionEmitter->SetEmitCount(mPoofCount);
    mProgressionEmitter->SetEmitRate(mPoofEmitRate);
  }
}
