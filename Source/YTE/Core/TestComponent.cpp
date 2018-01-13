#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Core/TestComponent.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  YTEDefineType(TestComponent)
  {
    YTERegisterType(TestComponent);
  }

  TestComponent::TestComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<TestComponent*>(aProperties, this, TestComponent::GetStaticType());
  }

  TestComponent::~TestComponent()
  {
  }

  void TestComponent::Initialize()
  {
    String hud{ "test" };
    auto engine = mSpace->GetEngine();
    auto hudSpace = engine->AddComposition<Space>("HudLevel", engine, nullptr);
    hudSpace->Load(engine->GetLevel(hud));

    auto transform = mOwner->GetComponent<Transform>();

    mCurrentPosition = transform->GetTranslation();

    mCurrentScale = 1.0;
    //mCurrentRotation = glm::vec3(glm::pi<float>() / 6.0f,
    //                             glm::pi<float>() / 4.0f,
    //                             glm::pi<float>() / 3.0f);
    //
    //transform->SetRotationProperty(mCurrentRotation);


    mTime = mCurrentPosition.z < 0.0f ? glm::pi<float>() : 0.0f;

    mSpace->YTERegister(Events::LogicUpdate, this, &TestComponent::Update);
  }

  void TestComponent::Update(LogicUpdate *aEvent)
  {
    auto transform = mOwner->GetComponent<Transform>();
    mTime += aEvent->Dt;

    static const float speed = 2.0f;

    //auto mod = glm::cos(mTime);
    //
    //mCurrentPosition.y = mod;
    //
    //mCurrentScale = ((mod)+2.0f) / 2.0f;
    //
    //glm::vec3 scale{ mCurrentScale, mCurrentScale, mCurrentScale };

    mCurrentRotation.y = mTime;

    //printf("%s: %f\n", mOwner->GetName().c_str(), mCurrentRotation.y);

    //transform->SetTranslation(mCurrentPosition);
    //transform->SetScale(scale);
    transform->SetRotationProperty(mCurrentRotation);
  }
}