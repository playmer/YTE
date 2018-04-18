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
    , mPoofCount(15)
    , mPoofEmitRate(0.001)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ProgressionParticles::Initialize()
  {
    if (Composition *emitterObj = mOwner->FindFirstCompositionByName("ProgressionEmitter"))
    {
       mProgressionEmitter = emitterObj->GetComponent<ParticleEmitter>();
    }

    mSpace->YTERegister(Events::LogicUpdate, this, &ProgressionParticles::Update);

    mSpace->YTERegister(Events::PostcardUpdate, this, &ProgressionParticles::OnPostcardUpdate);
  }

  void ProgressionParticles::Update(LogicUpdate *aEvent)
  {
    if (mProgressionEmitter)
    {
      if (mMakePoof)
      {
        mProgressionEmitter->SetEmitCount(mPoofCount);
        mProgressionEmitter->SetEmitRate(mPoofEmitRate);
      }

      if (mTimer > mPoofTime)
      {
        mTimer = 0.0;
        mMakePoof = false;
        mProgressionEmitter->SetEmitCount(0);
        mProgressionEmitter->SetEmitRate(10000.0f);
      }
      else
      {
        mTimer += aEvent->Dt;
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

  void ProgressionParticles::OnPostcardUpdate(PostcardUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    MakePoof();
  }

  void ProgressionParticles::MakePoof()
  {
    mMakePoof = true;
  }
}
