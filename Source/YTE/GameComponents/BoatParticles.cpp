/******************************************************************************/
/*!
\file   BoatParticles.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-03-22

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BoatParticles.hpp"
#include "YTE/GameComponents/BoatController.hpp"

#include "YTE/Graphics/ParticleEmitter.hpp"

#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Orientation.hpp"

namespace YTE
{
  YTEDefineType(BoatParticles)
  {
    YTERegisterType(BoatParticles);

    YTEBindProperty(&BoatParticles::GetVarianceScalar, &BoatParticles::SetVarianceScalar, "VarianceScalar")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BoatParticles::GetFrontWeight, &BoatParticles::SetFrontWeight, "FrontWeight")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BoatParticles::GetSideWeight, &BoatParticles::SetSideWeight, "SideWeight")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    std::vector<std::vector<Type*>> deps =
    {
      { TypeId<Transform>() },
      { TypeId<Orientation>() },
      { TypeId<RigidBody>() }
    };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  BoatParticles::BoatParticles(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mVarianceScalar(1.0f)
    , mFrontWeight(0.5f)
    , mSideWeight(0.5f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void BoatParticles::Initialize()
  {
    mRigidBody = mOwner->GetComponent<RigidBody>();
    mOrientation = mOwner->GetComponent<Orientation>();
    mTransform = mOwner->GetComponent<Transform>();
    mBoatController = mOwner->GetComponent<BoatController>();

    if (Composition *frontRight = mOwner->FindFirstCompositionByName("FrontRightEmitter"))
    {
      mFrontRightEmitter = frontRight->GetComponent<ParticleEmitter>();
    }
    
    if (Composition *frontLeft = mOwner->FindFirstCompositionByName("FrontLeftEmitter"))
    {
      mFrontLeftEmitter = frontLeft->GetComponent<ParticleEmitter>();
    }

    if (Composition *sideRight = mOwner->FindFirstCompositionByName("SideRightEmitter"))
    {
      mSideRightEmitter = sideRight->GetComponent<ParticleEmitter>();
    }

    if (Composition *sideLeft = mOwner->FindFirstCompositionByName("SideLeftEmitter"))
    {
      mSideLeftEmitter = sideLeft->GetComponent<ParticleEmitter>();
    }

    if (Composition *front = mOwner->FindFirstCompositionByName("FrontEmitter"))
    {
      mFrontEmitter = front->GetComponent<ParticleEmitter>();
    }

    if (Composition *back = mOwner->FindFirstCompositionByName("BackEmitter"))
    {
      mBackEmitter = back->GetComponent<ParticleEmitter>();
    }

    mOwner->GetSpace()->YTERegister(Events::LogicUpdate, this, &BoatParticles::Update);
  }

  void BoatParticles::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);

    float boatSpeed = mBoatController->GetCurrentSpeed();

    if (boatSpeed < 0.2f)
    {
      boatSpeed = 0.0f;
    }

    int emitCount = 15 * static_cast<int>(boatSpeed);
    
    if (emitCount > 45)
    {
      emitCount = 45;
    }

    //float emitRate = 0.00001f; //1.0001f - 2.0f * (boatSpeed / mBoatController->GetMaxSailSpeed());

    glm::vec3 forward = 0.5f * mOrientation->GetForwardVector();
    glm::vec3 right = mOrientation->GetRightVector();

    glm::vec3 variance = mVarianceScalar * sqrt(boatSpeed) * glm::vec3(1.0f);

    // front right
    glm::vec3 weightedRight = mFrontWeight * right;
    mFrontRightEmitter->SetInitVelocity(boatSpeed * (forward + weightedRight + glm::vec3(0.0f, 0.5f, 0.0f)));
    mFrontRightEmitter->SetVelocityVariance(variance);

    // front left
    mFrontLeftEmitter->SetInitVelocity(boatSpeed * (forward - weightedRight + glm::vec3(0.0f, 0.5f, 0.0f)));
    mFrontLeftEmitter->SetVelocityVariance(variance);

    // side right
    glm::vec3 weightedSide = mSideWeight * right;
    mSideRightEmitter->SetInitVelocity(boatSpeed * (forward + weightedSide + glm::vec3(0.0f, 0.3f, 0.0f)));
    mSideRightEmitter->SetVelocityVariance(variance);

    // side left
    mSideLeftEmitter->SetInitVelocity(boatSpeed * (forward - weightedSide + glm::vec3(0.0f, 0.3f, 0.0f)));
    mSideLeftEmitter->SetVelocityVariance(variance);

    // front
    mFrontEmitter->SetInitVelocity(boatSpeed * (1.3f * forward + glm::vec3(0.0f, 0.3f, 0.0f)));
    mFrontEmitter->SetVelocityVariance(variance);

    // back
    mBackEmitter->SetInitVelocity(glm::vec3(0));
    //mBackEmitter->SetVelocityVariance(variance);
  }

  float BoatParticles::GetVarianceScalar() const
  {
    return mVarianceScalar;
  }

  void BoatParticles::SetVarianceScalar(float aScalar)
  {
    mVarianceScalar = aScalar;
  }

  float BoatParticles::GetFrontWeight() const
  {
    return mFrontWeight;
  }

  void BoatParticles::SetFrontWeight(float aWeight)
  {
    mFrontWeight = aWeight;
  }

  float BoatParticles::GetSideWeight() const
  {
    return mSideWeight;
  }

  void BoatParticles::SetSideWeight(float aWeight)
  {
    mSideWeight = aWeight;
  }
}
