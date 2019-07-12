#pragma once

#include <random>

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ImguiLayer.hpp"
#include "YTE/Graphics/Material.hpp"

namespace YTE
{
  class SteppingStone : public Component
  {
  public:
    YTEDeclareType(SteppingStone);

    SteppingStone(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Start() override;

    void Update(LogicUpdate *aUpdate);

  private:
    void AllocateObjects();

    std::vector<glm::vec3> mColors;
    std::vector<std::vector<Composition*>> mSprites;
    Composition *mSpriteOwner;

    ImguiLayer *mLayer;
    int mK;
    int mN;

    std::random_device r;
    std::default_random_engine e{ r() };
  };
}