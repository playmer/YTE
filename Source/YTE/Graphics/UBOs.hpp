///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#ifndef YTE_Graphics_UBOs_hpp
#define YTE_Graphics_UBOs_hpp

#include "YTE/Core/Utilities.hpp"
#include "YTE/Math/Constants.hpp"

#define YTE_Graphics_LightCount 64
#define YTE_Graphics_WaterInformationCount 128

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
    glm::mat4 mProjectionMatrix = mat4Identity;
    glm::mat4 mViewMatrix = mat4Identity;
    glm::vec4 mCameraPosition = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
  };

  struct UBOModel
  {
    glm::mat4 mModelMatrix = mat4Identity;
    glm::vec4 mDiffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // tempDiffuseColorComponent
  };

  enum class UBOMaterialFlags : unsigned int
  {
    IsGizmo    = 1 << 0,
    IsSelected = 1 << 1,
  };

  struct UBOMaterial
  {
    UBOMaterial(glm::vec4 aDiffuse,
                glm::vec4 aAmbient,
                glm::vec4 aSpecular,
                glm::vec4 aEmissive,
                glm::vec4 aTransparent,
                glm::vec4 aReflective,
                float aOpacity,
                float aShininess,
                float aShininessStrength,
                float aReflectivity,
                float aReflectiveIndex,
                float aBumpScaling)
      : mDiffuse(aDiffuse)
      , mAmbient(aAmbient)
      , mSpecular(aSpecular)
      , mEmissive(aEmissive)
      , mTransparent(aTransparent)
      , mReflective(aReflective)
      , mOpacity(aOpacity)
      , mShininess(aShininess)
      , mShininessStrength(aShininessStrength)
      , mReflectivity(aReflectivity)
      , mReflectiveIndex(aReflectiveIndex)
      , mBumpScaling(aBumpScaling)
    {

    }

    UBOMaterial()
    {

    }
    
    glm::vec4 mDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mAmbient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mSpecular = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mEmissive = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mTransparent = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mReflective = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    float mOpacity = 1.0f;
    float mShininess = 1.0f;
    float mShininessStrength = 1.0f;
    float mReflectivity = 1.0f;
    float mReflectiveIndex = 1.0f;
    float mBumpScaling = 1.0f;
    u32 mFlags = 0;
    i32 mUsesNormalTexture = 0;
  };


  struct UBOAnimation
  {
    UBOAnimation()
    {
      glm::mat4 identity{1.0f};
      for (auto i = 0; i < BoneConstants::MaxBones; ++i)
      {
        mBones[i] = identity;
      }
    }

    glm::mat4 mBones[BoneConstants::MaxBones];
    u32 mHasAnimation = 0;    // 0 is false, 1 is true
  };


  struct UBOWaterInfluenceMap
  {
    glm::vec3 mColor;
    float mColorIntensity;
    glm::vec3 mCenter;
    float mRadius;
    float mWaveIntensity;
    unsigned int mColorInfluenceFunction;
    unsigned int mWaveInfluenceFunction;
    unsigned int mActive;
  };


  struct UBOWaterInformationMan
  {
    UBOWaterInfluenceMap mInformation[YTE_Graphics_WaterInformationCount];
    unsigned int mNumberOfInfluences = 0;
    float mBaseHeight;
    glm::vec2 mPadding;
  };


  struct UBOLight
  {
    glm::vec3 mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    float mActive = 0.0f; // bool
    glm::vec4 mDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mAmbient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 mSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    float mIntensity = 1.0f;
    glm::vec2 mSpotLightConeAngles = glm::vec2(0.0f, 0.0f); // (inner, outer)
    unsigned int mLightType = 0;
    float mSpotLightFalloff = 0.0f;
  };

  struct UBOIllumination
  {
    glm::vec4 mCameraPosition = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mGlobalIllumination = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mFogColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mFogCoefficients = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec2 mFogPlanes = glm::vec2(0.0f, 0.0f);
    float mTime;
    float mPadding;
  };

  struct UBOLightMan
  {
    UBOLight mLights[YTE_Graphics_LightCount];
    unsigned int mNumOfLights = 0;
    float mActive = 0.0f;  // bool
  };
}

#endif
