/******************************************************************************/
/*!
\file   PostcardIconPulse.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-03-22

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/PostcardIconPulse.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Core/Engine.cpp"

namespace YTE
{
  YTEDefineType(PostcardIconPulse)
  {
    YTERegisterType(PostcardIconPulse);
  }

  PostcardIconPulse::PostcardIconPulse(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mIsPulsing(false)
    , mInitialScale(glm::vec3(0))
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void PostcardIconPulse::Initialize()
  {
    mTransform = mOwner->GetComponent<Transform>();
    mInitialScale = mTransform->GetWorldScale();

    mSpace->GetOwner()->YTERegister(Events::LogicUpdate, this, &PostcardIconPulse::Update);
  }

  void PostcardIconPulse::Update(LogicUpdate *aEvent)
  {
    if (mIsPulsing)
    {
      mTimer += 5.0f * aEvent->Dt;
      float sizeScalar = sin(mTimer) + 1.0f;

      sizeScalar *= 0.15f;

      mTransform->SetWorldScale(((1.0f + sizeScalar) * mInitialScale));
    }
    else
    {
      mTransform->SetWorldScale(mInitialScale);
    }
  }

  void PostcardIconPulse::SetPulsing(bool aIsPulsing)
  {
    mIsPulsing = aIsPulsing;
    mTimer = 0.0f;
  }
}
