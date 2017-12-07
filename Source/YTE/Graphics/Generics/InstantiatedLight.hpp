///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_InstantiatedLight_hpp
#define YTE_Graphics_Generics_InstantiatedLight_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  enum class LightType
  {
    None = 0,         // Light will not be calculated with
    Directional = 1,  // Based on Direction only, no position is used
    Point = 2,        // Position based light, no direction used
    Spot = 3,         // Uses the Spot Light Cone Angles, and is position and direction based
    //Area = 4,       // ?
  };

  

  class InstantiatedLight : public EventHandler
  {
  public:
    YTEDeclareType(InstantiatedLight);

    InstantiatedLight();

    void SetLightSourceInformation(UBOLight &aLight)
    {
      mLightUBOData = aLight;
      mDataChanged = true;
    }

    void SetPosition(const glm::vec3 aPosition)
    {
      mLightUBOData.mPosition = glm::vec4(aPosition, 1.0f);
      mDataChanged = true;
    }

    void SetDirection(const glm::vec3& aDirection)
    {
      mLightUBOData.mDirection = glm::vec4(aDirection, 0.0f);
      mDataChanged = true;
    }

    void SetAmbient(const glm::vec3& aColor)
    {
      mLightUBOData.mAmbient = glm::vec4(aColor, 1.0f);
      mDataChanged = true;
    }

    void SetDiffuse(const glm::vec3& aColor)
    {
      mLightUBOData.mDiffuse = glm::vec4(aColor, 1.0f);
      mDataChanged = true;
    }

    void SetSpecular(const glm::vec3& aColor)
    {
      mLightUBOData.mSpecular = glm::vec4(aColor, 1.0f);
      mDataChanged = true;
    }

    void SetSpotLightCones(const glm::vec2& aCones) // inner, outer
    {
      mLightUBOData.mSpotLightConeAngles = aCones;
      mDataChanged = true;
    }

    void SetSpotLightFalloff(const float& aFalloff)
    {
      mLightUBOData.mSpotLightFalloff = aFalloff;
      mDataChanged = true;
    }

    void SetLightType(const LightType aLightType)
    {
      mLightUBOData.mLightType = static_cast<unsigned int>(aLightType);
      mDataChanged = true;
    }



    float GetSpotLightFalloff()
    {
      return mLightUBOData.mSpotLightFalloff;
    }

    glm::vec3 GetPosition() const
    {
      return glm::vec3(mLightUBOData.mPosition);
    }

    glm::vec3 GetDirection() const
    {
      return glm::vec3(mLightUBOData.mDirection);
    }

    glm::vec3 GetAmbient() const
    {
      return glm::vec3(mLightUBOData.mAmbient);
    }

    glm::vec3 GetDiffuse() const
    {
      return glm::vec3(mLightUBOData.mDiffuse);
    }

    glm::vec3 GetSpecular() const
    {
      return glm::vec3(mLightUBOData.mSpecular);
    }

    glm::vec2 GetSpotLightCones() const
    {
      return mLightUBOData.mSpotLightConeAngles;
    }

    LightType GetLightType() const
    {
      return static_cast<LightType>(mLightUBOData.mLightType);
    }

    UBOLight* GetLightData()
    {
      return &mLightUBOData;
    }



    UBOLight mLightUBOData;
    bool mDataChanged;
  };
}


#endif