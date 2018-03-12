#include "YTE/GameComponents/Helpers/TriangleData.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "YTE/GameComponents/Helpers/BoatPhysicsMath.hpp"

#pragma optimize("", off)

namespace YTE
{
  TriangleData::TriangleData(glm::vec3 aP1, glm::vec3 aP2, glm::vec3 aP3m, RigidBody* boatRB, Transform* boatTrans, FFT_WaterSimulation* aSim, float timeSinceStart) : p1(aP1)
    , p2(aP2)
    , p3(aP3m)
  {
    (void)timeSinceStart;
    center = (p1 + p2 + p3) / 3.0f;
    auto height = aSim->GetHeight(center.x, center.z);
    distanceToSurface = std::abs(center.y - height);

    glm::vec3 v1 = p2 - p1;
    glm::vec3 v2 = p3 - p1;
    normal = glm::normalize(glm::cross(v1, v2));

    area = BoatPhysicsMath::GetTriangleArea(p1, p2, p3);
    velocity = BoatPhysicsMath::GetTriangleVelocity(boatRB, boatTrans, center);
    velocityDir = glm::normalize(velocity);
    cosTheta = glm::dot(velocityDir, normal);
  }

}



