#include "YTE/GameComponents/Helpers/BoatPhysicsMath.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"
#include "glm/gtx/vector_angle.hpp"


namespace YTE
{

  const float BoatPhysicsMath::cRhoOcean = 12.70f;
  const float BoatPhysicsMath::cRhoAir = 1.225f;

  glm::vec3 BoatPhysicsMath::GetTriangleVelocity(RigidBody* boatRB, Transform* boatTrans, glm::vec3 triCenter)
  {
    glm::vec3 v_B = boatRB->GetVelocity();
    glm::vec3 omega_B = boatRB->GetAngularVelocity();
    glm::vec3 r_BA = triCenter - boatTrans->GetWorldTranslation();
    glm::vec3 v_A = v_B + glm::cross(omega_B, r_BA);
    return v_A;
  }

  float BoatPhysicsMath::GetTriangleArea(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
  {
    float a = glm::distance(p1, p2);
    float c = glm::distance(p3, p1);
    float areaSin = (a * c * std::sin(glm::angle(p2 - p1, p3 - p1))) * 0.5f;
    return areaSin;
  }

  glm::vec3 BoatPhysicsMath::BuoyancyForce(float rho, float gravityY, const TriangleData& triangleData)
  {
    glm::vec3 bF = rho * gravityY * triangleData.distanceToSurface * triangleData.area * triangleData.normal;

    bF = CheckForceIsValid(bF, "Buoyancy");

    bF.x = 0.0f;
    bF.z = 0.0f;
    
    //if (bF.y < 0.1f)
    //{
    //  bF.y = 0.0f;
    //}

    return bF;
  }

  glm::vec3 BoatPhysicsMath::ViscousWaterResistanceForce(float rho, const TriangleData& triangleData, float Cf)
  {
    glm::vec3 B = triangleData.normal;
    glm::vec3 A = triangleData.velocity;

    glm::vec3 velocityTangent = glm::cross(B, glm::cross(A, B) / glm::length(B)) / glm::length(B);
    glm::vec3 tangentialDirection = glm::normalize(velocityTangent) * -1.0f;
    glm::vec3 v_f_vec = glm::length(triangleData.velocity) * tangentialDirection;
    glm::vec3 vwrF = 0.5f * rho * glm::length(v_f_vec) * v_f_vec * triangleData.area * Cf;
    vwrF = CheckForceIsValid(vwrF, "Viscous Water Resistance Force");
    return vwrF;
  }

  float BoatPhysicsMath::ResistanceCoefficient(float rho, float velocity, float length)
  {
    (void)rho;
    float nu = 0.000001f;
    float Rn = (velocity * length) / nu;
    float Cf = 0.075f / std::pow((std::log10(Rn) - 2.0f), 2.0f);
    return Cf;
  }

  glm::vec3 BoatPhysicsMath::PressureDragForce(const TriangleData& triangleData)
  {
    float velocity = glm::length(triangleData.velocity);

    glm::vec3 pressureDragForce;
    if (triangleData.cosTheta > 0.0f)
    {
      float C_PD1 = 10.0f;
      float C_PD2 = 10.0f;
      float f_P = 0.5f;

      pressureDragForce = -(C_PD1 * velocity + C_PD2 * (velocity * velocity)) * triangleData.area * std::pow(triangleData.cosTheta, f_P) * triangleData.normal;
    }
    else
    {
      float C_SD1 = 10.0f;
      float C_SD2 = 10.0f;
      float f_S = 0.5f;

      pressureDragForce = (C_SD1 * velocity + C_SD2 * (velocity * velocity)) * triangleData.area * std::pow(std::abs(triangleData.cosTheta), f_S) * triangleData.normal;
    }

    pressureDragForce = CheckForceIsValid(pressureDragForce, "Pressure drag");

    return pressureDragForce;
  }

  glm::vec3 BoatPhysicsMath::SlammingForce(const SlammingForceData& slammingData, const TriangleData& triangleData, float boatArea, float boatMass, float dt)
  {
    if (triangleData.cosTheta < 0.0f || slammingData.originalArea <= 0.0f)
    {
      return glm::vec3();
    }

    glm::vec3 dV = slammingData.submergedArea * slammingData.velocity;
    glm::vec3 dV_previous = slammingData.previousSubmergedArea * slammingData.previousVelocity;
    glm::vec3 accVec = (dV - dV_previous) / (slammingData.originalArea * dt);
    float acc = glm::length(accVec);
    glm::vec3 F_stop = boatMass * triangleData.velocity * ((2.0f * triangleData.area) / boatArea);
    float p = 2.0f;
    float acc_max = acc;
    float slammingCheat = 1.0f;
    auto clamp = [](float a, float min, float max)->float { return std::max(std::min(a, max), min); };
    glm::vec3 slammingForce = std::pow(clamp(acc / acc_max, 0, 1), p) * triangleData.cosTheta * F_stop * slammingCheat;
    slammingForce *= -1.0f;

    slammingForce = CheckForceIsValid(slammingForce, "Slamming");

    return slammingForce;
  }

  float BoatPhysicsMath::ResidualResistanceForce()
  {
    float residualResistanceForce = 0.0f;

    return residualResistanceForce;
  }

  glm::vec3 BoatPhysicsMath::AirResistanceForce(float rho, const TriangleData& triangleData, float C_air)
  {
    if (triangleData.cosTheta < 0.0f)
    {
      return glm::vec3();
    }

    //Find air resistance force
    glm::vec3 airResistanceForce = 0.5f * rho * glm::length(triangleData.velocity) * triangleData.velocity * triangleData.area * C_air;

    //Acting in the opposite side of the velocity
    airResistanceForce *= -1.0f;

    airResistanceForce = CheckForceIsValid(airResistanceForce, "Air resistance");

    return airResistanceForce;
  }

  glm::vec3 BoatPhysicsMath::CheckForceIsValid(glm::vec3 force, std::string forceName)
  {
    if (!std::isnan(force.x + force.y + force.z))
    {
      return force;
    }
    return glm::vec3();
  }
}
