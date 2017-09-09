#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"


namespace YTE
{
  class TestComponent : public Component
  {
  public:
    DeclareType(TestComponent);

    TestComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    void Update(LogicUpdate *aEvent);

    ~TestComponent() override;

  private:
    glm::vec3 mCurrentPosition;
    float mCurrentScale;
    glm::vec3 mCurrentRotation;
    float mTime;
  };
}

