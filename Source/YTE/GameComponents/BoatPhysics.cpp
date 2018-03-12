#include "BoatPhysics.hpp"
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
  }

  BoatPhysics::BoatPhysics(Composition *aOwner, Space * aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , waterDensity(BoatPhysicsMath::RHO_OCEAN)
    , airDensity(BoatPhysicsMath::RHO_AIR)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void BoatPhysics::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &BoatPhysics::Update);
    boatRB = mOwner->GetComponent<RigidBody>();
    transform = mOwner->GetComponent<Transform>();
    orientation = mOwner->GetComponent<Orientation>();
    model = mOwner->GetComponent<Model>();

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

    mDebugDrawer->Start();

    for (auto &tri : modifyBoatMesh.UnderwaterTriangleData())
    {
      mDebugDrawer->AddTriangle(tri.p1, tri.p2, tri.p3);
    }

    mDebugDrawer->End();
    
    glm::vec3 under, over;
    if (modifyBoatMesh.UnderwaterTriangleData().size() > 0)
    {
      under = AddUnderwaterForces(waterSim, (float)aLogicUpdate->Dt);
    }
    //if (modifyBoatMesh.AbovewaterTriangleData().size() > 0)
    //{
    //  over = AddAbovewaterForces(waterSim);
    //}

    glm::vec3 total = under + over;
    std::cout << "under: " << under.x << ", " << under.y << ", " << under.z << std::endl;
    std::cout << "over: " << over.x << ", " << over.y << ", " << over.z << std::endl;
    std::cout << "total: " << total.x << ", " << total.y << ", " << total.z << std::endl;
  }

  glm::vec3 BoatPhysics::AddUnderwaterForces(FFT_WaterSimulation* aSim, float dt)
  {
    glm::vec3 total;
    (void)aSim;
    float Cf = BoatPhysicsMath::ResistanceCoefficient(waterDensity, glm::length(boatRB->GetVelocity()), modifyBoatMesh.CalculateUnderwaterLength());
    auto slammingForceData = modifyBoatMesh.SlammingData();
    CalculateSlammingVelocities(slammingForceData);
    float boatArea = modifyBoatMesh.BoatArea();
    float boatMass = boatRB->GetMass();

    auto indexOfOriginalTriangle = modifyBoatMesh.IndexOfOrigTris();
    auto underwaterTriangleData = modifyBoatMesh.UnderwaterTriangleData();
    for (int i = 0; i < underwaterTriangleData.size(); ++i)
    {
      TriangleData triangleData = underwaterTriangleData[i];
      glm::vec3 forceToAdd;
      forceToAdd += BoatPhysicsMath::BuoyancyForce(waterDensity, boatRB->GetGravity().y, triangleData);
      //forceToAdd += BoatPhysicsMath::ViscousWaterResistanceForce(waterDensity, triangleData, Cf);
      //forceToAdd += BoatPhysicsMath::PressureDragForce(triangleData);
      //int originalTriangleIndex = indexOfOriginalTriangle[i];
      //SlammingForceData slammingData = slammingForceData[originalTriangleIndex];
      //
      //forceToAdd += BoatPhysicsMath::SlammingForce(slammingData, triangleData, boatArea, boatMass, dt);

      total += forceToAdd;
      // Add the forces to the boat
      boatRB->ApplyForce(forceToAdd, triangleData.center - transform->GetWorldTranslation());
    }

    return total;
  }

  glm::vec3 BoatPhysics::AddAbovewaterForces(FFT_WaterSimulation* aSim)
  {
    /*
    glm::vec3 total;
    (void)aSim;
    auto ubo = model->GetInstantiatedModel()->GetUBOModelData();
    auto m2w = ubo.mModelMatrix;
    auto aboveWaterTriangleData = modifyBoatMesh.AbovewaterTriangleData();

    for (int i = 0; i < aboveWaterTriangleData.size(); ++i)
    {
      TriangleData triangleData = aboveWaterTriangleData[i];

      // Calculate the forces
      glm::vec3 forceToAdd;

      // Force 1 - Air resistance
      //forceToAdd += BoatPhysicsMath::AirResistanceForce(airDensity, triangleData, 1.28f);

      // Add the forces to the boat
      boatRB->ApplyForce(forceToAdd, triangleData.center - transform->GetWorldTranslation());
      total += forceToAdd;
    }
    */
    return glm::vec3(0.0f, 0.0f, 0.0f);
  }

  void BoatPhysics::CalculateSlammingVelocities(std::vector<SlammingForceData>& slammingForceData)
  {
    /*
    auto ubo = model->GetInstantiatedModel()->GetUBOModelData();
    auto m2w = ubo.mModelMatrix;
    for (int i = 0; i < slammingForceData.size(); ++i)
    {
      // Set the new velocity to the old velocity
      slammingForceData[i].previousVelocity = slammingForceData[i].velocity;

      // Center of the triangle in world space
      glm::vec4 c = m2w * glm::vec4(slammingForceData[i].triangleCenter, 1);
      glm::vec3 center(c.x, c.y, c.z);

      slammingForceData[i].velocity = BoatPhysicsMath::GetTriangleVelocity(boatRB, transform, center);
    }
    */
  }

}



