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
  vec4 mPosition;
  vec4 mDirection;
  vec4 mAmbient;
  vec4 mDiffuse;
  vec4 mSpecular;
  vec2 mSpotLightConeAngles;    // (inner, outer)
  uint mLightType;  // 0 = none, 1 = directional, 2 = point, 3 = spot, 4 = area
  // area not in use right now
  float mSpotLightFalloff;
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

// ========================
// Material Values
layout (binding = 2) uniform UBOMaterial
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
    vec2 mPadding;    // not used
} Material;

// ========================
// Light Values
layout (binding = 3) uniform UBOLights
{
  Light mLights[MAX_LIGHTS];
  uint mNumberOfLights;
} Lights;

// ========================
// Illumination Buffer
layout (binding = 4) uniform UBOIllumination
{
  vec4 mCameraPosition;
  vec4 mGlobalIllumination;
  vec4 mFogColor;
  vec4 mFogCoefficients;
  vec2 mFogPlanes;
} Illumination;



///////////////////////////////////////////////////////////////////////////////
// Samplers
layout (binding = 5) uniform sampler2D diffuseSampler;
layout (binding = 6) uniform sampler2D specularSampler;


///////////////////////////////////////////////////////////////////////////////
// Fragment Shader Inputs | Vertex Shader Outputs
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;

// ========================
// Output of Fragment
layout (location = 0) out vec4 outFragColor;










///////////////////////////////////////////////////////////////////////////////
// Functions

// ======================
// Main:
// Entry Point of Shader
void main()
{
  outFragColor = texture(specularSampler, inTextureCoordinates);
  outFragColor = texture(diffuseSampler, inTextureCoordinates);
    outFragColor = vec4(1,0,0,1);

}
