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

    YTE_Shared InstantiatedLight();

    void SetLightSourceInformation(UBOs::Light &aLight)
    {
      mLightUBOData = aLight;
      mDataChanged = true;
    }

    void SetPosition(const glm::vec3 aPosition)
    {
      mLightUBOData.mPosition = aPosition;
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
      mLightUBOData.mSpecular = aColor;
      mDataChanged = true;
    }

    void SetSpotLightCones(const glm::vec2& aCones) // inner, outer
    {
      mLightUBOData.mSpotLightConeAngles = glm::vec2(glm::radians(aCones.x), glm::radians(aCones.y));
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

    void SetActive(const bool aValue)
    {
      mLightUBOData.mActive = aValue ? 10.0f : 0.0f;
      mDataChanged = true;
    }

    void SetIntensity(const float aValue)
    {
      mLightUBOData.mIntensity = aValue;
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
      return glm::vec2(glm::degrees(mLightUBOData.mSpotLightConeAngles.x), glm::degrees(mLightUBOData.mSpotLightConeAngles.y));
    }

    LightType GetLightType() const
    {
      return static_cast<LightType>(mLightUBOData.mLightType);
    }

    UBOs::Light* GetLightData()
    {
      return &mLightUBOData;
    }

    bool GetActive()
    {
      return mLightUBOData.mActive;
    }

    float GetIntensity()
    {
      return mLightUBOData.mIntensity;
    }

    UBOs::Light mLightUBOData;
    bool mDataChanged;
  };
}


#endif
