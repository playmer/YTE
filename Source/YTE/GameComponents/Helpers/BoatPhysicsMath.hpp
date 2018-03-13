#include "YTE/Core/Utilities.hpp"
#include "YTE/GameComponents/Helpers/SlammingForceData.hpp"
#include "YTE/GameComponents/Helpers/TriangleData.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class BoatPhysicsMath
  {
  public:
    static const float cRhoOcean;
    static const float cRhoAir;

    static glm::vec3 GetTriangleVelocity(RigidBody* boatRB, Transform* boatTrans, glm::vec3 triCenter);
    static float GetTriangleArea(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    static glm::vec3 BuoyancyForce(float rho, float gravityY, const TriangleData& triangleData);
    static glm::vec3 ViscousWaterResistanceForce(float rho, const TriangleData& triangleData, float Cf);
    static float ResistanceCoefficient(float rho, float velocity, float length);
    static glm::vec3 PressureDragForce(const TriangleData& triangleData);
    static glm::vec3 SlammingForce(const SlammingForceData& slammingData, const TriangleData& triangleData, float boatArea, float boatMass, float dt);
    static float ResidualResistanceForce();
    static glm::vec3 AirResistanceForce(float rho, const TriangleData& triangleData, float C_air);
    static glm::vec3 CheckForceIsValid(glm::vec3 force, std::string forceName);
  };
}
