#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class ParticleEmitter : public Component
  {
  public:
    YTEDeclareType(ParticleEmitter);
    ParticleEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~ParticleEmitter();

    void Initialize() override;


  private:

    glm::vec3 mStartVelocity;
     
    glm::vec3 mStartPosOffset;



  };
}
