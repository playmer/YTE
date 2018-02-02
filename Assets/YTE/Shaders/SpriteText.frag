#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable



///////////////////////////////////////////////////////////////////////////////
// Defines
#define MAX_LIGHTS 64


///////////////////////////////////////////////////////////////////////////////
// Structures
struct Light
{
  vec3 mPosition;
  float mActive;
  vec4 mDirection;
  vec4 mAmbient;
  vec4 mDiffuse;
  vec4 mSpecular;
  vec2 mSpotLightConeAngles;    // (inner, outer)
  uint mLightType;  // 0 = none, 1 = directional, 2 = point, 3 = spot, 4 = area
  // area not in use right now
  float mSpotLightFalloff;
};

// used to pass data to lighting functions
struct LightingData
{
  vec4 mDiffuseTexture;
  vec4 mNormalTexture;
  vec4 mSpecularTexture;
  vec4 mViewVec;
  vec4 mNormal;
  vec4 mPosition;
};


///////////////////////////////////////////////////////////////////////////////
// Constants
const uint LightType_None = 0;
const uint LightType_Directional = 1;
const uint LightType_Point = 2;
const uint LightType_Spot = 3;
//const uint LightType_Area = 4; // area not in use




///////////////////////////////////////////////////////////////////////////////
// UBO Buffers

//====================
// Material Values
layout (binding = UBO_MATERIAL_BINDING) uniform UBOMaterial
{
    vec4 mDiffuse;
    vec4 mAmbient;
    vec4 mSpecular;
    vec4 mEmissive;
    vec4 mTransparent;
    vec4 mReflective;
    float mOpacity;
    float mShininess;
    float mShininessStrength;
    float mReflectivity;
    float mReflectiveIndex;
    float mBumpScaling;
    int mIsEditorObject;
    float mPadding; // unused
} Material;

// ========================
// Light Values
layout (binding = UBO_LIGHTS_BINDING) uniform UBOLights
{
  Light mLights[MAX_LIGHTS];
  uint mNumberOfLights;
  float mActive;
} Lights;

// ========================
// Illumination Buffer
layout (binding = UBO_ILLUMINATION_BINDING) uniform UBOIllumination
{
  vec4 mCameraPosition;
  vec4 mGlobalIllumination;
  vec4 mFogColor;
  vec4 mFogCoefficients;
  vec2 mFogPlanes;
} Illumination;



///////////////////////////////////////////////////////////////////////////////
// Samplers
layout (binding = UBO_DIFFUSE_BINDING) uniform sampler2D diffuseSampler;


layout (location = 0) in vec2 inTextureCoordinates;
layout (location = 1) in vec4 inDiffuse;

// ========================
// Output of Fragment
layout (location = 0) out vec4 outFragColor;

// ======================
// Main:
// Entry Point of Shader
void main()
{
  vec4 texColor = texture(diffuseSampler, inTextureCoordinates);

  if (texColor != vec4(0.0f, 0.0f, 0.0f, 1.0f))
  {
    if (Material.mIsEditorObject > 0)
    {
      outFragColor = texColor;
    }
    else
    {
      outFragColor = texColor * inDiffuse;
    }
  }

  else
  {
    outFragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  }
  //outFragColor = vec4(inTextureCoordinates.y, 0.0f, 0.0f, 1.0f);
}