#pragma once

#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  class SlammingForceData
  {
  public:
    float originalArea;
    float submergedArea;
    float previousSubmergedArea;
    glm::vec3 triangleCenter;
    glm::vec3 velocity;
    glm::vec3 previousVelocity;
  };
}