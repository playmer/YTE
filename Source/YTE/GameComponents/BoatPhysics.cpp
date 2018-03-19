#include "YTE/GameComponents/BoatPhysics.hpp"
#include "YTE/GameComponents/Helpers/BoatPhysicsMath.hpp"

#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/RigidBody.hpp"

#include <cmath>

#pragma optimize("", off)

namespace YTE
{
  YTEDefineType(BoatPhysics)
  {
    YTERegisterType(BoatPhysics);

    std::vector<std::vector<Type*>> deps = { 
      { TypeId<Model>() },
      { TypeId<RigidBody>() } 
    };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
    
  }

  BoatPhysics::BoatPhysics(Composition *aOwner, Space * aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , waterDensity(BoatPhysicsMath::cRhoOcean)
    , airDensity(BoatPhysicsMath::cRhoAir)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void BoatPhysics::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &BoatPhysics::Update);
    mBoatRB = mOwner->GetParent()->GetComponent<RigidBody>();
    mBoatTransform = mOwner->GetParent()->GetComponent<Transform>();
    mBoatOrientation = mOwner->GetParent()->GetComponent<Orientation>();

    mTransform = mOwner->GetComponent<Transform>();
    mModel = mOwner->GetComponent<Model>();

    modifyBoatMesh = ModifyBoatMesh(mOwner);

    auto gv = mOwner->GetSpace()->GetComponent<GraphicsView>();

    mDebugDrawer = new TriangleDrawer("boatDebugDraw", gv->GetRenderer(), gv);
  }

  void BoatPhysics::Update(LogicUpdate* aLogicUpdate)
  {
    YTEUnusedArgument(aLogicUpdate);
    Composition * obj = mSpace->FindFirstCompositionByName("WaterFFT");
    FFT_WaterSimulation * waterSim = obj->GetComponent<FFT_WaterSimulation>();

    modifyBoatMesh.GenerateUnderwaterMesh(waterSim, (float)aLogicUpdate->Dt);


    glm::vec3 under, over;
    if (modifyBoatMesh.UnderwaterTriangleData().size() > 0)
    {
      under = AddUnderwaterForces(waterSim, (float)aLogicUpdate->Dt);
    }
    
    //if (modifyBoatMesh.AbovewaterTriangleData().size() > 0)
    //{
    //  over = AddAbovewaterForces(waterSim);
    //}
  }

  glm::vec3 BoatPhysics::AddUnderwaterForces(FFT_WaterSimulation* aSim, float dt)
  {
    glm::vec3 total;
    (void)aSim;
    (void)dt;
    //float Cf = BoatPhysicsMath::ResistanceCoefficient(waterDensity, 
    //                                                  glm::length(mBoatRB->GetVelocity()), 
    //                                                  modifyBoatMesh.CalculateUnderwaterLength());
    auto slammingForceData = modifyBoatMesh.SlammingData();
    CalculateSlammingVelocities(slammingForceData);
    //float boatArea = modifyBoatMesh.BoatArea();
    //float boatMass = mBoatRB->GetMass();

    auto indexOfOriginalTriangle = modifyBoatMesh.IndexOfOrigTris();
    auto underwaterTriangleData = modifyBoatMesh.UnderwaterTriangleData();
    for (int i = 0; i < underwaterTriangleData.size(); ++i)
    {
      TriangleData triangleData = underwaterTriangleData[i];
      glm::vec3 forceToAdd;
      forceToAdd += BoatPhysicsMath::BuoyancyForce(waterDensity, mBoatRB->GetGravity().y, triangleData);
      //forceToAdd += BoatPhysicsMath::ViscousWaterResistanceForce(waterDensity, triangleData, Cf);
      //forceToAdd += BoatPhysicsMath::PressureDragForce(triangleData);
      int originalTriangleIndex = indexOfOriginalTriangle[i];
      SlammingForceData slammingData = slammingForceData[originalTriangleIndex];
      //
      //forceToAdd += BoatPhysicsMath::SlammingForce(slammingData, triangleData, boatArea, boatMass, dt);
    
      total += forceToAdd;
      // Add the forces to the boat
      //boatRB->ApplyForce(forceToAdd, glm::vec3(0.0f));
      mBoatRB->ApplyForce(forceToAdd, glm::vec3(0.0f));
    }

    auto velocity = mBoatRB->GetVelocity();

    float velY = velocity.y;
    velY *= 0.8;
    mBoatRB->SetVelocity(velocity.x, velY, velocity.z);

    //auto trans = mTransform->GetWorldTranslation();
    //
    //auto height = aSim->GetHeight(trans.x, trans.z);
    //
    //float heightDiff = trans.y - height.y;
    //
    //float bF = 1270.0f * 10.0f * heightDiff;
    //
    //glm::vec3 force(0.0f, bF, 0.0f);
    //
    //mBoatRB->ApplyForce(force, glm::vec3(0.0f));

    return total;
  }

  glm::vec3 BoatPhysics::AddAbovewaterForces(FFT_WaterSimulation* aSim)
  {
    (void)aSim;
    
    glm::vec3 total;
    auto ubo = mModel->GetInstantiatedModel()[0]->GetUBOModelData();
    auto m2w = ubo.mModelMatrix;
    auto aboveWaterTriangleData = modifyBoatMesh.AbovewaterTriangleData();

    for (int i = 0; i < aboveWaterTriangleData.size(); ++i)
    {
      TriangleData triangleData = aboveWaterTriangleData[i];

      // Calculate the forces
      glm::vec3 forceToAdd;

      // Force 1 - Air resistance
      forceToAdd += BoatPhysicsMath::AirResistanceForce(airDensity, triangleData, 1.28f);

      // Add the forces to the boat
      mBoatRB->ApplyForce(forceToAdd, glm::vec3(0.0f));
      total += forceToAdd;
    }
    
    return glm::vec3(0.0f, 0.0f, 0.0f);
  }

  void BoatPhysics::CalculateSlammingVelocities(std::vector<SlammingForceData>& slammingForceData)
  {
    (void)slammingForceData;
    
    auto ubo = mModel->GetInstantiatedModel()[0]->GetUBOModelData();
    auto m2w = ubo.mModelMatrix;
    for (int i = 0; i < slammingForceData.size(); ++i)
    {
      // Set the new velocity to the old velocity
      slammingForceData[i].previousVelocity = slammingForceData[i].velocity;

      // Center of the triangle in world space
      glm::vec4 c = m2w * glm::vec4(slammingForceData[i].triangleCenter, 1);
      glm::vec3 center(c.x, c.y, c.z);

      slammingForceData[i].velocity = BoatPhysicsMath::GetTriangleVelocity(mBoatRB, mTransform, center);
    }
    
  }

}



