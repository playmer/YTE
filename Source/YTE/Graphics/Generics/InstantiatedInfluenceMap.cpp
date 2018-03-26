///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedInfluenceMap.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedInfluenceMap)
  {
    YTERegisterType(InstantiatedInfluenceMap);
  }



  InstantiatedInfluenceMap::InstantiatedInfluenceMap()
  {
    mDataChanged = true;
    mInfluenceMapUBOData.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    mInfluenceMapUBOData.mCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    mInfluenceMapUBOData.mRadius = 1.0f;
  }
}


