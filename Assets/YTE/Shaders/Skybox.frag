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
  vec3 mDirection;
  vec3 mAmbient;
  vec3 mDiffuse;
  vec3 mSpecular;
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
    vec3 mDiffuse;
    vec3 mAmbient;
    vec3 mSpecular;
    vec3 mEmissive;
    vec3 mTransparent;
    vec3 mReflective;
    float mOpacity;
    float mShininess;
    float mShininessStrength;
    float mReflectivity;
    float mReflectiveIndex;
    float mBumpScaling;
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
  vec3 mCameraPosition;
  vec3 mGlobalIllumination;
  vec3 mFogColor;
  vec3 mFogCoefficients;
  vec2 mFogPlanes;
} Illumination;


///////////////////////////////////////////////////////////////////////////////
// Samplers
layout (binding = 5) uniform samplerCube environmentMap;


///////////////////////////////////////////////////////////////////////////////
// Fragment Shader Inputs | Vertex Shader Outputs
layout (location = 0) in vec3 inTextureCoordinates;

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
  //vec3 normal = normalize(inTextureCoordinates);
  //outFragColor = texture(environmentMap, normal);
  vec3 add = vec3(1.0f, 1.0f, 1.0f);
  outFragColor = vec4(add + inTextureCoordinates, 1.0f);
}
