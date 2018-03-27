///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_InstantiatedInfluenceMap_hpp
#define YTE_Graphics_Generics_InstantiatedInfluenceMap_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class InstantiatedInfluenceMap : public EventHandler
  {
  public:
    YTEDeclareType(InstantiatedInfluenceMap);

    InstantiatedInfluenceMap();

    void SetInfluenceMapInformation(UBOWaterInfluenceMap &aMap)
    {
      mInfluenceMapUBOData = aMap;
      mDataChanged = true;
    }

    void SetRadius(float &aRadius)
    {
      mInfluenceMapUBOData.mRadius = aRadius;
      mDataChanged = true;
    }

    void SetCenter(glm::vec3 aCenter)
    {
      mInfluenceMapUBOData.mCenter = aCenter;
      mDataChanged = true;
    }

    void SetColor(glm::vec3 &aColor)
    {
      mInfluenceMapUBOData.mColor = aColor;
      mDataChanged = true;
    }

    void SetColorIntensity(float aIntensity)
    {
      mInfluenceMapUBOData.mColorIntensity = aIntensity;
      mDataChanged = true;
    }

    void SetWaveIntensity(float aIntensity)
    {
      mInfluenceMapUBOData.mWaveIntensity = aIntensity;
      mDataChanged = true;
    }

    void SetActive(bool aActive)
    {
      if (aActive)
      {
        mInfluenceMapUBOData.mActive = 100;
      }
      else
      {
        mInfluenceMapUBOData.mActive = 0;
      }
      mDataChanged = true;
    }

    void SetWaveInfluenceFunction(unsigned int aFunction)
    {
      mInfluenceMapUBOData.mWaveInfluenceFunction = aFunction;
      mDataChanged = true;
    }

    void SetColorInfluenceFunction(unsigned int aFunction)
    {
      mInfluenceMapUBOData.mColorInfluenceFunction = aFunction;
      mDataChanged = true;
    }



    float GetRadius()
    {
      return mInfluenceMapUBOData.mRadius;
    }

    glm::vec3 GetCenter()
    {
      return mInfluenceMapUBOData.mCenter;
    }

    glm::vec3 GetColor()
    {
      return mInfluenceMapUBOData.mColor;
    }

    float GetColorIntensity()
    {
      return mInfluenceMapUBOData.mColorIntensity;
    }

    float GetWaveIntensity()
    {
      return mInfluenceMapUBOData.mWaveIntensity;
    }

    bool GetActive()
    {
      if (mInfluenceMapUBOData.mActive > 0)
      {
        return true;
      }
      return false;
    }

    unsigned int GetWaveInfluenceFunction()
    {
      return mInfluenceMapUBOData.mWaveInfluenceFunction;
    }

    unsigned int GetColorInfluenceFunction()
    {
      return mInfluenceMapUBOData.mColorInfluenceFunction;
    }


    UBOWaterInfluenceMap mInfluenceMapUBOData;
    bool mDataChanged;
  };
}


#endif
