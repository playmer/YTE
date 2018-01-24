///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#ifndef YTE_Graphics_UBOs_hpp
#define YTE_Graphics_UBOs_hpp

#include "YTE/Core/Utilities.hpp"

#define YTE_Graphics_LightCount 64

namespace YTE
{
  namespace BoneConstants
  {
    // NOTE: If a change in this value is made,
    //       a change in the vertex structure must be made as well
    const int MaxBonesPerVertex{ 5 };

    // NOTE: These are for looping over the individual bone data elements
    //       (BoneWeights and BoneWeights2)
    const int MaxBonesPerVertex1{ 3 };
    const int MaxBonesPerVertex2{ 2 };


    // NOTE: If a change in this value is made,
    //       a change in the shaders must be made as well
    const int MaxBones{ 64 };
  }

  struct UBOView
  {
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
  };

  struct UBOModel
  {
    glm::mat4 mModelMatrix;
    glm::vec4 mDiffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // tempDiffuseColorComponent
  };

  struct UBOMaterial
  {
    glm::vec4 mDiffuse;
    glm::vec4 mAmbient;
    glm::vec4 mSpecular;
    glm::vec4 mEmissive;
    glm::vec4 mTransparent;
    glm::vec4 mReflective;
    float mOpacity = 1.0f;
    float mShininess = 1.0f;
    float mShininessStrength = 1.0f;
    float mReflectivity = 1.0f;
    float mReflectiveIndex = 1.0f;
    float mBumpScaling = 1.0f;
    int mIsEditorObject = 0;
    float mPadding = 0.0f;
  };


  struct UBOAnimation
  {
    glm::mat4 mBones[BoneConstants::MaxBones];
    bool mHasAnimation;    // 0 is false, 1 is true
  };


  struct UBOLight
  {
    glm::vec3 mPosition;
    float mActive; // bool
    glm::vec4 mDirection;
    glm::vec4 mAmbient;
    glm::vec4 mDiffuse;
    glm::vec4 mSpecular;
    glm::vec2 mSpotLightConeAngles; // (inner, outer)
    unsigned int mLightType;
    float mSpotLightFalloff;
  };

  struct UBOIllumination
  {
    glm::vec4 mCameraPosition;
    glm::vec4 mGlobalIllumination;
    glm::vec4 mFogColor;
    glm::vec4 mFogCoefficients;
    glm::vec2 mFogPlanes;
  };

  struct UBOLightMan
  {
    UBOLight mLights[YTE_Graphics_LightCount];
    unsigned int mNumOfLights;
    float mActive;  // bool
  };
}

#endif
