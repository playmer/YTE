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
layout (binding = 7) uniform sampler2D normalSampler;
layout (location = 3) in vec4 inPositionWorld;
layout (location = 4) in vec4 inPosition;


///////////////////////////////////////////////////////////////////////////////
// Fragment Shader Inputs | Vertex Shader Outputs
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;






///////////////////////////////////////////////////////////////////////////////
// Functions

vec4 saturate(vec4 aColor)
{
  return clamp(aColor, 0.0f, 1.0f);
}

// ======================
// Calc_DirectionalLight:
// Calculates as a directional light with the given light
vec4 Calc_DirectionalLight(inout Light aLight, inout LightingData aLightData)
{
  vec4 lightVec = -aLight.mDirection;

  // diffuse
  float diffContribution = dot(lightVec, aLightData.mNormalTexture);

  // used later
  vec4 specularColor = vec4(0.0f);
  vec4 diffuseColor = vec4(0.0f);

  // actually saves computation to have this conditional
  if (diffContribution > 0.0f)
  {
    diffuseColor = aLight.mDiffuse * diffContribution * aLightData.mDiffuseTexture;

    // specular
    vec4 reflectVec = reflect(-lightVec, aLightData.mNormalTexture);
    float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), Material.mShininess);
    specularColor = aLight.mSpecular * aLightData.mSpecularTexture * specContribution;
  }

  // ambient
  vec4 ambientColor = aLight.mAmbient * Material.mAmbient;

  // combine
  return saturate(diffuseColor + ambientColor + specularColor);
}


// ======================
// Calc_PointLight:
// Calculates as a point light with the given light
vec4 Calc_PointLight(inout Light aLight, inout LightingData aLightData)
{
  return vec4(0, 0, 0, 0);
}


// ======================
// Calc_SpotLight:
// Calculates as a spot light with the given light
vec4 Calc_SpotLight(inout Light aLight, inout LightingData aLightData)
{
  return vec4(0, 0, 0, 0);
}


// ======================
// CalculateLight:
// Finds what light to calculate as
vec4 CalculateLight(int i, inout LightingData aLightData)
{
  Light light = Lights.mLights[i];
  
  // find type of light and call corresponding function
  if (light.mLightType == LightType_Directional)
  {
    return Calc_DirectionalLight(light, aLightData);
  }
  else if(light.mLightType == LightType_Point)
  {
    return Calc_PointLight(light, aLightData);
  }
  else if(light.mLightType == LightType_Spot)
  {
    return Calc_SpotLight(light, aLightData);
  }

  return vec4(0,0,0,0);
}


// ======================
// SampleTextures:
// Calculates the phong illumination for fragment
LightingData SampleTextures(vec2 aUV, inout vec4 aNormal)
{
  LightingData lightData;
  lightData.mDiffuseTexture  = texture(diffuseSampler, aUV);
  lightData.mSpecularTexture = texture(specularSampler, aUV);
  lightData.mNormalTexture   = texture(normalSampler, aUV);
  return lightData;
}



// ======================
// Phong:
// Calculates the phong illumination for fragment
vec4 Phong(vec4 aNormal, vec4 aPosition, vec2 aUV)
{
  // Emissive and Global Illumination
  vec4 ITotal = Material.mEmissive +
                (Illumination.mGlobalIllumination * Material.mAmbient);

  // view vector
  vec4 viewVec = (Illumination.mCameraPosition - aPosition);

  // atmo attenuation scalar
  float scalar = (Illumination.mFogPlanes.y - length(viewVec)) /
                 (Illumination.mFogPlanes.y - Illumination.mFogPlanes.x);

  // length no longer needed, normalized vector needed for light calculations
  viewVec = normalize(viewVec);

  // Sample all textures at our position and fill out lighting data
  LightingData lightData = SampleTextures(aUV, aNormal);
  lightData.mViewVec = viewVec;
  lightData.mNormal = aNormal;
  lightData.mPosition = aPosition;

  // Light Calculations
  for (int i = 0; i < Lights.mNumberOfLights; ++i)
  {
    ITotal += CalculateLight(i, lightData);
  }

  // final color
  return (scalar * ITotal) + ((1.0f - scalar) * Illumination.mFogColor);
}


// ======================
// Main:
// Entry Point of Shader
void main()
{
  if (Lights.mNumberOfLights == 0)
  {
    outFragColor = texture(diffuseSampler, inTextureCoordinates.xy);
  }
  else
  {
    outFragColor = Phong(vec4(normalize(inNormal), 0.0f), inPositionWorld, inTextureCoordinates.xy);
  }
}