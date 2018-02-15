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
  vec3 mSpecular;
  float mIntensity;
  vec2 mSpotLightConeAngles;    // (inner, outer)
  uint mLightType;  // 0 = none, 1 = directional, 2 = point, 3 = spot, 4 = area
  // area not in use right now
  float mSpotLightFalloff;
};

// used to pass data to lighting functions
struct LightingData
{
  // diffuse
  vec4 mDiffTexture;
  vec4 mDiffMat;

  // normal
  vec4 mNormalTexture;

  // specular
  vec4 mSpecTexture;
  vec4 mSpecMat;

  // emissive
  vec4 mEmisMat;

  // ambient
  vec4 mAmbMat;

  // shinniness
  float mShininessMat;

  // misc calculating values
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
// Submesh Material Values
layout (binding = UBO_SUBMESH_MATERIAL_BINDING) uniform UBOSubmeshMaterial
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
    int mUsesNormalTexture;
} SubmeshMaterial;

//====================
// Model Material Values
layout (binding = UBO_MODEL_MATERIAL_BINDING) uniform UBOModelMaterial
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
    int mUsesNormalTexture;
} ModelMaterial;

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
layout (binding = UBO_DIFFUSE_BINDING) uniform sampler2D diffuseSampler;  // diff
layout (binding = UBO_SPECULAR_BINDING) uniform sampler2D specularSampler; // foam
layout (binding = UBO_NORMAL_BINDING) uniform sampler2D normalSampler;   // noise


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
  float diffContribution = max(dot(lightVec, aLightData.mNormal), 0.0f);
  vec4 diffuseColor = aLight.mDiffuse * diffContribution * aLightData.mDiffTexture;
  
  // specular
  vec4 reflectVec = reflect(-lightVec, aLightData.mNormal);
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat);
  vec4 specularColor = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution;

  // ambient
  vec4 ambientColor = aLight.mAmbient * aLightData.mAmbMat / 10.0f;

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
  vec4 ambientColor = aLight.mAmbient * aLightData.mAmbMat / 10.0f;
  
  // diffuse
  float diffContribution = max(dot(lightVec, aLightData.mNormal), 0.0f);
  vec4 diffuseColor = aLight.mDiffuse * diffContribution * aLightData.mDiffTexture;

  // specular
  vec4 reflectVec = reflect(-lightVec, aLightData.mNormal);
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat);
  vec4 specularColor = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution;

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
  vec4 ambientColor = aLight.mAmbient * aLightData.mAmbMat / 10.0f;

  // diffuse
  float diffContribution = max(dot(aLightData.mNormal, lightVec), 0.0f);
  vec4 diffuseColor = diffContribution * aLight.mDiffuse * aLightData.mDiffTexture;

  // specular
  vec4 reflectVec = reflect(-lightVec, aLightData.mNormal);
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat);
  vec4 specularColor = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution;

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


// ===================
// Foam:
// calculates the foam
vec4 Foam(vec4 aOriginalColor, float aNormalX, vec2 aUV)
{
  // TODO: make foam a texture, and valley a texture
  //       make base height modifyable
  //       make max and min heights modifyable
  //       make intensity modifyable

  float baseHeight = 0.0f;
  float minHeight = -1.0f;
  float maxHeight = 0.5f;
  float intensityFoam = 5.0f;
  float intensityValley = 1.0f;
  //vec4 foamColor = vec4(0.75f, 0.75f, 1.0f, 1.0f);
  vec4 foamColor = texture(specularSampler, aUV);
  vec4 valleyColor = vec4(0.25f, 0.25f, 0.5f, 0.5f);

  // foam
  // h - baseHeight / MaxHeight - baseHeight
  if (aNormalX > 0.0f)
  {
    float mixamount = saturate((saturate((inPositionWorld.y - baseHeight) / (maxHeight - baseHeight)) * aNormalX) * intensityFoam);
    aOriginalColor = mix(aOriginalColor,foamColor, mixamount);
  }
  else
  {
    float mixamount = (-saturate((inPositionWorld.y - baseHeight) / (minHeight -baseHeight)) * aNormalX) * intensityValley;
    aOriginalColor = mix(aOriginalColor,-valleyColor, mixamount);
  }

  return aOriginalColor;
}


// ======================
// saturatehigh:
// interval of 0.25-1.0
float saturatehigh(float aValue)
{
  return clamp(aValue, 0.1f, 1.0f);
}


// ======================
// SampleTextures:
// Calculates the phong illumination for fragment
LightingData SampleTextures(vec2 aUV, inout vec4 aNormal, vec4 aViewVec)
{
  LightingData lightData;

  // diffuse
  lightData.mDiffMat = SubmeshMaterial.mDiffuse * ModelMaterial.mDiffuse;
  lightData.mDiffTexture  = texture(diffuseSampler, aUV + (aNormal.xy / 10.0f)) * lightData.mDiffMat;

  // specular
  lightData.mSpecMat = SubmeshMaterial.mSpecular * ModelMaterial.mSpecular;
  lightData.mSpecTexture = ModelMaterial.mSpecular * SubmeshMaterial.mSpecular * lightData.mSpecMat; //texture(specularSampler, aUV);

  // normal
  lightData.mNormalTexture   = normalize(mix(aNormal, aViewVec, saturatehigh(texture(normalSampler, aUV + (aNormal.xy / 10.0f)).x - 0.75f)));
  //lightData.mNormalTexture = aNormal;
  lightData.mDiffTexture  = Foam(lightData.mDiffTexture, lightData.mNormalTexture.x, aUV + (aNormal.xy / 10.0f));

  // emissive
  lightData.mEmisMat = SubmeshMaterial.mEmissive * ModelMaterial.mEmissive;

  // ambient
  lightData.mAmbMat = SubmeshMaterial.mAmbient * ModelMaterial.mAmbient;

  // shinniness
  lightData.mShininessMat = SubmeshMaterial.mShininess + ModelMaterial.mShininess;

  return lightData;
}


// ======================
// Phong:
// Calculates the phong illumination for fragment
vec4 Phong(vec4 aNormal, vec4 aPosition, vec4 aPositionWorld, vec2 aUV)
{
  // view vector in view space for attenuation
  vec4 viewVec = ((inViewMatrix * Illumination.mCameraPosition) - aPosition);

  // atmo attenuation scalar
  float scalar = (Illumination.mFogPlanes.y - length(viewVec)) /
                 (Illumination.mFogPlanes.y - Illumination.mFogPlanes.x);
  scalar = saturate(scalar);

  // view vector in world space for lighting calculations
  viewVec = normalize(Illumination.mCameraPosition - aPositionWorld);

  // Sample all textures at our position and fill out lighting data
  LightingData lightData = SampleTextures(aUV, aNormal, viewVec);
  lightData.mViewVec = viewVec;
  lightData.mNormal = aNormal;
  lightData.mPosition = aPositionWorld - texture(normalSampler, aUV + (aNormal.xy / 10.0f));

  // Emissive and Global Illumination
  vec4 ITotal = lightData.mEmisMat +
                (Illumination.mGlobalIllumination * lightData.mAmbMat);

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
    outFragColor = texture(diffuseSampler, inTextureCoordinates.xy + (inNormal.xy / 10.0f));
    outFragColor = outFragColor * SubmeshMaterial.mDiffuse * ModelMaterial.mDiffuse;
    outFragColor = Foam(outFragColor, vec4(normalize(inNormal), 0.0f).x, inTextureCoordinates.xy + (inNormal.xy / 10.0f));
  }
  else
  {
    outFragColor = Phong(vec4(normalize(inNormal), 0.0f), inPosition, vec4(inPositionWorld, 1.0f), inTextureCoordinates.xy);
  }
}
