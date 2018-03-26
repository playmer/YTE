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

    void SetCenter(glm::vec3 &aCenter)
    {
      mInfluenceMapUBOData.mCenter = aCenter;
      mDataChanged = true;
    }

    void SetColor(glm::vec3 &aColor)
    {
      mInfluenceMapUBOData.mColor = glm::vec4(aColor, 1.0f);
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
      return glm::vec3(mInfluenceMapUBOData.mColor);
    }


    UBOWaterInfluenceMap mInfluenceMapUBOData;
    bool mDataChanged;
  };
}


#endif
