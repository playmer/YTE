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

// ========================
// View matrix
layout (binding = 0) uniform UBOView
{
  mat4 mProjectionMatrix;
  mat4 mViewMatrix;
} View;

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
  float mActive;
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
layout (location = 3) in vec4 inPosition;
layout (location = 4) in vec3 inPositionWorld;
layout (location = 5) in mat4 inViewMatrix;

// ========================
// Output of Fragment
layout (location = 0) out vec4 outFragColor;










///////////////////////////////////////////////////////////////////////////////
// Functions

// ======================
// Saturate:
// Clamps a given value to the interval [0-1]
vec4 saturate(vec4 aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}
vec3 saturate(vec3 aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}
vec2 saturate(vec2 aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}
float saturate(float aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}


// ======================
// Calc_DirectionalLight:
// Calculates as a directional light with the given light
vec4 Calc_DirectionalLight(inout Light aLight, inout LightingData aLightData)
{
  vec4 lightVec = normalize(-aLight.mDirection);

  // diffuse
  float diffContribution = max(dot(lightVec, aLightData.mNormalTexture), 0.0f);
  vec4 diffuseColor = aLight.mDiffuse * diffContribution * aLightData.mDiffuseTexture;
  
  // specular
  vec4 reflectVec = reflect(-lightVec, aLightData.mNormalTexture);
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), Material.mShininess);
  vec4 specularColor = aLight.mSpecular * aLightData.mSpecularTexture * specContribution;

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
  vec4 lightVec = vec4(aLight.mPosition, 1.0f) - aLightData.mPosition;
  float lightVecDistance = length(lightVec);
  
  lightVec = normalize(lightVec);
  
  // ambient
  vec4 ambientColor = aLight.mAmbient * Material.mAmbient;
  
  // diffuse
  float diffContribution = max(dot(lightVec, aLightData.mNormalTexture), 0.0f);
  vec4 diffuseColor = aLight.mDiffuse * diffContribution * aLightData.mDiffuseTexture;

  // specular
  vec4 reflectVec = reflect(-lightVec, aLightData.mNormalTexture);
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), Material.mShininess);
  vec4 specularColor = aLight.mSpecular * aLightData.mSpecularTexture * specContribution;

  // attenuation
  float att = 1.0f / ( (Illumination.mFogCoefficients.x) +
                       (Illumination.mFogCoefficients.y * lightVecDistance) +
                       (Illumination.mFogCoefficients.z * (lightVecDistance * lightVecDistance)) );
  
  return saturate((diffuseColor + specularColor + ambientColor) * att);
}


// ======================
// Calc_SpotLight:
// Calculates as a spot light with the given light
vec4 Calc_SpotLight(inout Light aLight, inout LightingData aLightData)
{
  vec4 lightVec = (inViewMatrix * vec4(aLight.mPosition, 1.0f)) - (inViewMatrix * aLightData.mPosition);
  float lightVecDistance = length(lightVec);
  lightVec = normalize(lightVec);

  // ambient
  vec4 ambientColor = aLight.mAmbient * Material.mAmbient;

  // diffuse
  float diffContribution = max(dot(aLightData.mNormalTexture, lightVec), 0.0f);
  vec4 diffuseColor = diffContribution * aLight.mDiffuse * aLightData.mDiffuseTexture;

  // specular
  vec4 reflectVec = reflect(-lightVec, aLightData.mNormalTexture);
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), Material.mShininess);
  vec4 specularColor = aLight.mSpecular * aLightData.mSpecularTexture * specContribution;

  // attenuation
  float att = min(1.0f / (Illumination.mFogCoefficients.x +
                          Illumination.mFogCoefficients.y * lightVecDistance +
                          Illumination.mFogCoefficients.z * (lightVecDistance * lightVecDistance)), 1.0f);
  
  // spotlight effect
  vec4 spotlightDirection = normalize(inViewMatrix * -aLight.mDirection);
  float alpha = dot(spotlightDirection, lightVec);
  float phi = cos(aLight.mSpotLightConeAngles.y);
  float theta = cos(aLight.mSpotLightConeAngles.x);
  float spotEffect = 0.0f;

  if (alpha < phi)
  {
    spotEffect = 0.0f;
  }
  else if (alpha > theta)
  {
    spotEffect = 1.0f;
  }
  else
  {
    spotEffect = pow((alpha - phi) / (theta - phi), aLight.mSpotLightFalloff);
  }

  return saturate(att * ambientColor + att * spotEffect * (diffuseColor + specularColor));
}


// ======================
// CalculateLight:
// Finds what light to calculate as
vec4 CalculateLight(int i, inout LightingData aLightData)
{
  Light light = Lights.mLights[i];

  if (light.mActive < 0.5f)
  {
    return vec4(0,0,0,0);
  }
  
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
  lightData.mNormalTexture   = aNormal;
  return lightData;
}


// ======================
// Phong:
// Calculates the phong illumination for fragment
vec4 Phong(vec4 aNormal, vec4 aPosition, vec4 aPositionWorld, vec2 aUV)
{
  // Emissive and Global Illumination
  vec4 ITotal = Material.mEmissive +
                (Illumination.mGlobalIllumination * Material.mAmbient);

  // view vector in view space for attenuation
  vec4 viewVec = ((inViewMatrix * Illumination.mCameraPosition) - aPosition);

  // atmo attenuation scalar
  float scalar = (Illumination.mFogPlanes.y - length(viewVec)) /
                 (Illumination.mFogPlanes.y - Illumination.mFogPlanes.x);
  scalar = saturate(scalar);

  // view vector in world space for lighting calculations
  viewVec = normalize(Illumination.mCameraPosition - aPositionWorld);

  // Sample all textures at our position and fill out lighting data
  LightingData lightData = SampleTextures(aUV, aNormal);
  lightData.mViewVec = viewVec;
  lightData.mNormal = aNormal;
  lightData.mPosition = aPositionWorld;

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
  if (Lights.mActive < 0.5f)
  {
    outFragColor = texture(diffuseSampler, inTextureCoordinates.xy);
  }
  else
  {
    outFragColor = Phong(vec4(normalize(inNormal), 0.0f), inPosition, vec4(inPositionWorld, 1.0f), inTextureCoordinates.xy);
  }
}
