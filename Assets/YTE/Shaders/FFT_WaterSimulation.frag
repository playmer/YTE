#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable



///////////////////////////////////////////////////////////////////////////////
// Defines
#define MAX_LIGHTS 64
#define MAX_CLIP_PLANES 6


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
  vec3 mNoiseOffset;
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
  float mTime;
  float mMoveAmount;
} Illumination;


// ========================
// Clip Planes Buffer
layout (binding = UBO_CLIP_PLANES_BINDING) uniform UBOClipPlanes
{
  vec4 mPlanes[MAX_CLIP_PLANES];
  uint mNumberOfPlanes;
} ClipPlanes;



///////////////////////////////////////////////////////////////////////////////
// Samplers
layout (binding = UBO_DIFFUSE_BINDING) uniform sampler2D diffuseSampler;  // diff
layout (binding = UBO_SPECULAR_BINDING) uniform sampler2D specularSampler; // foam
layout (binding = UBO_NORMAL_BINDING) uniform sampler2D normalSampler;   // noise
layout (binding = UBO_FB1_BINDING) uniform sampler2D fbRefractiveSampler;
layout (binding = UBO_FB2_BINDING) uniform sampler2D fbReflectiveSampler;


///////////////////////////////////////////////////////////////////////////////
// Fragment Shader Inputs | Vertex Shader Outputs
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 inPosition;
layout (location = 4) in vec4 inClipSpace;
layout (location = 5) in vec3 inPositionWorld;
layout (location = 6) in mat4 inViewMatrix;

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






vec2 random2(vec2 st){
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

// Value Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

vec3 CalculateNoise(vec2 aUV)
{
    float t = Illumination.mTime * 0.125;
    vec2 st1 = vec2(aUV.x + t, aUV.y);
    vec2 st2 = vec2(aUV.x + t, aUV.y + t);
    vec2 st3 = vec2(aUV.x, aUV.y + t);
    vec2 st4 = vec2(aUV.x + t, aUV.y - t);

    vec3 color = vec3(0.0);

    vec2 pos1 = vec2(st1*50.0);
    vec2 pos2 = vec2(st2*20.0);
    vec2 pos3 = vec2(st3*100.0);
    vec2 pos4 = vec2(st4*70.0);

    color += vec3( noise(pos1)*.5+.5 );
    color += vec3( noise(pos2)*.5+.5 );
    color += vec3( noise(pos3)*.5+.5 );
    color += vec3( noise(pos4)*.5+.5 );

    return color * 0.25;
}


// ======================
// Calc_DirectionalLight:
// Calculates as a directional light with the given light
vec4 Calc_DirectionalLight(inout Light aLight, inout LightingData aLightData)
{
  vec4 lightVec = normalize(-aLight.mDirection);
  vec4 lightVecNew = normalize(-vec4(vec3(aLight.mDirection) + aLightData.mNoiseOffset, aLight.mDirection.w));

  // diffuse
  float diffContribution = max(dot(lightVec, aLightData.mNormal), 0.0f);
  vec4 diffuseColor = aLight.mDiffuse * diffContribution * aLightData.mDiffTexture;
  
  // specular
  vec4 reflectVec = normalize(reflect(-lightVecNew, aLightData.mNormal));
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat);
  float specContribution2 = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat / 10.0f);
  vec4 specularColor = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution;
  vec4 specularColor2 = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution2;
  //specularColor = mix(specularColor, specularColor2, ((specContribution - specContribution2) * 0.5f) + specContribution2);


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
  vec4 lightVecNew = vec4(aLight.mPosition + aLightData.mNoiseOffset, 1.0f) - aLightData.mPosition;

  vec4 lightVec = vec4(aLight.mPosition, 1.0f) - aLightData.mPosition;
  float lightVecDistance = length(lightVec);
  
  lightVec = normalize(lightVec);
  
  // ambient
  vec4 ambientColor = aLight.mAmbient * aLightData.mAmbMat / 10.0f;
  
  // diffuse
  float diffContribution = max(dot(lightVec, aLightData.mNormal), 0.0f);
  vec4 diffuseColor = aLight.mDiffuse * diffContribution * aLightData.mDiffTexture;

  // specular
  vec4 reflectVec = normalize(reflect(-lightVecNew, aLightData.mNormal));
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat);
  float specContribution2 = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat / 10.0f);
  vec4 specularColor = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution;
  vec4 specularColor2 = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution2;
  //specularColor = mix(specularColor, specularColor2, ((specContribution - specContribution2) * 0.5f) + specContribution2);

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
  vec4 lightVecNew = (inViewMatrix * vec4(aLight.mPosition + aLightData.mNoiseOffset, 1.0f)) - (inViewMatrix * aLightData.mPosition);

  // ambient
  vec4 ambientColor = aLight.mAmbient * aLightData.mAmbMat / 10.0f;

  // diffuse
  float diffContribution = max(dot(aLightData.mNormal, lightVec), 0.0f);
  vec4 diffuseColor = diffContribution * aLight.mDiffuse * aLightData.mDiffTexture;

  // specular
  vec4 reflectVec = normalize(reflect(-lightVecNew, aLightData.mNormal));
  float specContribution = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat);
  float specContribution2 = pow(max(dot(reflectVec, aLightData.mViewVec), 0.0f), aLightData.mShininessMat / 10.0f);
  vec4 specularColor = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution;
  vec4 specularColor2 = vec4(aLight.mSpecular, 1.0f) * aLightData.mSpecTexture * specContribution2;
  //specularColor = mix(specularColor, specularColor2, ((specContribution - specContribution2) * 0.5f) + specContribution2);

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
    return Calc_DirectionalLight(light, aLightData) * light.mIntensity;
  }
  else if(light.mLightType == LightType_Point)
  {
    return Calc_PointLight(light, aLightData) * light.mIntensity;
  }
  else if(light.mLightType == LightType_Spot)
  {
    return Calc_SpotLight(light, aLightData) * light.mIntensity;
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
  float maxHeight = 2.0f;
  float intensityFoam = 5.0f;
  float intensityValley = 1.0f;
  vec4 foamColor = vec4(0.75f, 0.75f, 1.0f, 1.0f);
  //vec4 foamColor = texture(specularSampler, aUV);
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
  // moves the textures around
  //vec2 uv = aUV + (aNormal.xy / 10.0f);
  vec2 uv = aUV;

  LightingData lightData;

  // diffuse
  lightData.mDiffMat = SubmeshMaterial.mDiffuse * ModelMaterial.mDiffuse;
  lightData.mDiffTexture  = texture(diffuseSampler, uv) * lightData.mDiffMat;

  // specular
  lightData.mSpecMat = SubmeshMaterial.mSpecular * ModelMaterial.mSpecular;
  lightData.mSpecTexture = vec4(1,1,1,1);//lightData.mSpecMat * texture(specularSampler, uv);

  // normal
  lightData.mNormalTexture   = aNormal;
  //lightData.mNormalTexture = aNormal;
  //lightData.mDiffTexture  = Foam(lightData.mDiffTexture, lightData.mNormalTexture.x, uv);

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
vec4 Phong(vec4 aNormal, vec4 aPosition, vec4 aPositionWorld, vec2 aUV, vec3 aNoiseOffset)
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
  lightData.mPosition = aPositionWorld;
  lightData.mNoiseOffset = aNoiseOffset;

  // Emissive and Global Illumination
  vec4 ITotal = lightData.mEmisMat +
                (Illumination.mGlobalIllumination * lightData.mDiffTexture);

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
  //vec4 plane = vec4(0, -1, 0, 5);
  //if (dot(plane, vec4(inPositionWorld, 1.0f)) < 0.0f)
  //{
  //  discard;
  //  return;
  //}

  if (ClipPlanes.mNumberOfPlanes > 0)
  {
    //outFragColor = vec4(0,0,1,0);
    discard;
    return;
  }

  float distortionStrength = 0.02f;
  vec2 ndc = (inClipSpace.xy/inClipSpace.w)/2.0f + 0.5f;
  vec2 refractCoords = vec2(ndc.x, ndc.y);
  vec2 reflectCoords = vec2(ndc.x, 1 - ndc.y);

  vec2 distA = (texture(normalSampler, vec2(inTextureCoordinates.x + Illumination.mTime * 0.5f, inTextureCoordinates.y)).rg) * distortionStrength;
  vec2 distB = (texture(normalSampler, vec2(-inTextureCoordinates.x + Illumination.mTime * 0.5f, inTextureCoordinates.y + Illumination.mTime * 0.5f)).rg) * distortionStrength;
  vec2 totalDist = distA + distB;
  refractCoords += totalDist;
  refractCoords = clamp(refractCoords, 0.001, 0.999);
  reflectCoords += totalDist;
  reflectCoords.x = clamp(reflectCoords.x, 0.001, 0.999);
  reflectCoords.y = clamp(reflectCoords.y, -0.999, -0.001);

  vec4 reflectColor = texture(fbReflectiveSampler, reflectCoords);
  vec4 refractColor = texture(fbRefractiveSampler, refractCoords);
  outFragColor = reflectColor;
  //outFragColor = mix(reflectColor, refractColor, 0.5f);
  outFragColor = mix(outFragColor, vec4(0.0f, 0.3f, 0.5f, 1.0f), 0.2f);
  //vec4 originalRefractiveColor = texture(fbRefractiveSampler, inTextureCoordinates.xy);
  //vec4 originalReflectionColor = texture(fbReflectiveSampler, inTextureCoordinates.xy);
  ////outFragColor = mix(originalReflectionColor, originalRefractiveColor, 0.5f);
  ////outFragColor = originalReflectionColor;

  //return;

  if (Lights.mActive < 0.5f)
  {
    outFragColor = texture(diffuseSampler, inTextureCoordinates.xy + (inNormal.xy / 10.0f));
    outFragColor = outFragColor * SubmeshMaterial.mDiffuse * ModelMaterial.mDiffuse;
    outFragColor = Foam(outFragColor, vec4(normalize(inNormal), 0.0f).x, inTextureCoordinates.xy + (inNormal.xy / 10.0f));
  }
  else
  {
    vec3 n = CalculateNoise(inTextureCoordinates.xy);
    vec4 posView = inPosition;
    posView.y += n.y;
    vec4 posWorld = vec4(inPositionWorld, 1.0f);
    posWorld.y += n.y;
    vec2 uv = inTextureCoordinates.xy;
    vec3 normal = normalize(inNormal) + n;
    normal = normalize(normal);
    //uv += n.xy;
    outFragColor = Phong(vec4(normal, 0.0f), posView, posWorld, uv, n);
  }
}
