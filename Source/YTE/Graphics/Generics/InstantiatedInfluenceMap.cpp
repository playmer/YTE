///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedInfluenceMap.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedInfluenceMap)
  {
    RegisterType<InstantiatedInfluenceMap>();
    TypeBuilder<InstantiatedInfluenceMap> builder;
  }



  InstantiatedInfluenceMap::InstantiatedInfluenceMap()
  {
    mDataChanged = true;
    mInfluenceMapUBOData.mColor = glm::vec3(1.0f, 1.0f, 1.0f);
    mInfluenceMapUBOData.mCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    mInfluenceMapUBOData.mRadius = 1.0f;
    mInfluenceMapUBOData.mColorIntensity = 1.0f;
    mInfluenceMapUBOData.mColorInfluenceFunction = 0;
    mInfluenceMapUBOData.mWaveInfluenceFunction = 0;
    mInfluenceMapUBOData.mActive = 10;
    mInfluenceMapUBOData.mWaveIntensity = 1.0f;
  }
}


