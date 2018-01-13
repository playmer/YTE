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
    mLightUBOData.mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    mLightUBOData.mActive = 10.0f;
    mLightUBOData.mDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    mLightUBOData.mDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    mLightUBOData.mAmbient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    mLightUBOData.mSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    mLightUBOData.mSpotLightConeAngles = glm::vec2(glm::radians(10.0f), glm::radians(20.0f));
    mLightUBOData.mSpotLightFalloff = 10.0f;
    mLightUBOData.mLightType = static_cast<unsigned int>(LightType::Point);
  }
}


