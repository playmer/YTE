#pragma once

#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class TriangleData
  {
  public:
    TriangleData(glm::vec3 aP1, glm::vec3 aP2, glm::vec3 aP3m, RigidBody* boatRB, Transform* boatTrans, FFT_WaterSimulation* aSim, float timeSinceStart);

    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;

    glm::vec3 center;
    
    float distanceToSurface;

    glm::vec3 normal;

    float area;

    glm::vec3 velocity;

    glm::vec3 velocityDir;

    float cosTheta;
  };
}
