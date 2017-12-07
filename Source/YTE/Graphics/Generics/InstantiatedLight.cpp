///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedLight.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedLight)
  {
    YTERegisterType(InstantiatedLight);
  }



  InstantiatedLight::InstantiatedLight()
  {
    mDataChanged = true;
    mLightUBOData.mPosition = glm::vec4(0, 0, 0, 0);
    mLightUBOData.mDirection = glm::vec4(0, 0, 0, 0);
    mLightUBOData.mDiffuse = glm::vec4(0, 0, 0, 0);
    mLightUBOData.mAmbient = glm::vec4(0, 0, 0, 0);
    mLightUBOData.mSpecular = glm::vec4(0, 0, 0, 0);
    mLightUBOData.mSpotLightConeAngles = glm::vec2(0, 0);
    mLightUBOData.mLightType = static_cast<unsigned int>(LightType::None);
  }
}


