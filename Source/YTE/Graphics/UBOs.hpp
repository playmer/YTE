///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#ifndef YTE_Graphics_UBOs_hpp
#define YTE_Graphics_UBOs_hpp

#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/Vertex.hpp"

namespace YTE
{
  struct UBOView
  {
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
  };



  struct UBOModel
  {
    glm::mat4 mModelMatrix;
  };


  
  struct UBOMaterial
  {
    glm::vec3 mDiffuse;
    glm::vec3 mAmbient;
    glm::vec3 mSpecular;
    glm::vec3 mEmissive;
    glm::vec3 mTransparent;
    glm::vec3 mReflective;
    float mOpacity = 1.0f;
    float mShininess = 1.0f;
    float mShininessStrength = 1.0f;
    float mReflectivity = 1.0f;
    float mReflectiveIndex = 1.0f;
    float mBumpScaling = 1.0f;
  };


  struct UBOAnimation
  {
    glm::mat4 mBones[BoneConstants::MaxBones];
    bool mHasAnimation;    // 0 is false, 1 is true
  };
}

#endif
