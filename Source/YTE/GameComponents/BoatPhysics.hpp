#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/GameComponents/Helpers/ModifyBoatMesh.hpp"
#include "YTE/GameComponents/Helpers/TriangleData.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Graphics/Drawers.hpp"

namespace YTE
{
  class BoatPhysics : public Component
  {
  public:
    YTEDeclareType(BoatPhysics);
    BoatPhysics(Composition *aOwner, Space * aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate* aLogicUpdate);
  private:
    glm::vec3 AddUnderwaterForces(FFT_WaterSimulation* aSim, float dt);
    glm::vec3 AddAbovewaterForces(FFT_WaterSimulation* aSim);
    void CalculateSlammingVelocities(std::vector<SlammingForceData>& slammingForceData);

    Transform * transform;
    RigidBody * boatRB;
    Orientation * orientation;
    Model * model;
    ModifyBoatMesh modifyBoatMesh;
    float waterDensity;
    float airDensity;

    TriangleDrawer *mDebugDrawer;
  };
}
