///////////////////////////////////////////////////////////////////////////////
// Defines
#define MAX_BONES 64
#define MAX_INFLUENCEMAPS 16


///////////////////////////////////////////////////////////////////////////////
// Vertex Layouts
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;


///////////////////////////////////////////////////////////////////////////////
// Structures
struct WaterInformation
{
  vec3 mColor;
  float mColorIntensity;
  vec3 mCenter;
  float mRadius;
  float mWaveIntensity;
  uint mColorInfluenceFunction;
  uint mWaveInfluenceFunction;
  uint mActive;
};


struct InfluenceMappedData
{
  vec3 mPos;
  vec3 mColor;
};



///////////////////////////////////////////////////////////////////////////////
// Constants
const uint InfluenceType_Linear = 0;
const uint InfluenceType_Squared = 1;
const uint InfluenceType_Cubic = 2;
const uint InfluenceType_Logarithmic = 3;


// ========================
// Model Matrix Buffer
layout (binding = UBO_MODEL_BINDING) uniform UBOModel
{
  mat4 mModelMatrix;
  vec4 mDiffuseColor;
} Model;


///////////////////////////////////////////////////////////////////////////////
// UBO Buffers

// ========================
// View Buffer
layout (binding = UBO_VIEW_BINDING) uniform UBOView
{
  mat4 mProjectionMatrix;
  mat4 mViewMatrix;
  vec4 mCameraPosition;
} View;


// ========================
// Animation Buffer
layout (binding = UBO_ANIMATION_BONE_BINDING) uniform UBOAnimation
{
  mat4 mBones[MAX_BONES];
  bool mHasAnimations;
} Animation;


// ========================
// Water Buffer
layout (binding = UBO_WATER_BINDING) uniform UBOWater
{
  WaterInformation mInfluenceMaps[MAX_INFLUENCEMAPS];
  uint mNumberOfInfluences;
  float mBaseHeight;
  vec2 mPadding;
} WaterInfo;


///////////////////////////////////////////////////////////////////////////////
// Vertex Shader Outputs | Fragment Shader Inputs
layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTextureCoordinates;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec4 outPosition;
layout (location = 4) out vec3 outPositionWorld;
layout (location = 5) out mat4 outViewMatrix;

// ========================
// Positional Output of Vertex
out gl_PerVertex 
{
    vec4 gl_Position;
};












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
// CalculatePosition:
// Position the vertex for the fragment shader and GPU
// takes the projection matrix, and the view position value to calculate with
void CalculatePosition(mat4 aProjMat, vec4 aPos)
{
  // Initial Position Update
  gl_Position = aProjMat        * 
                aPos;

  // Vulkan Specific Coordinate System Fix (fixes the depth of the vertex)
  //gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;  
}

// ======================
// CalculateNormal:
// Calculates the normal used for this vertex
// takes the view matrix, model matrix, and the normal value to calculate with
vec3 CalculateNormal(mat4 aViewMat, mat4 aModelMat, vec4 aNormal)
{
  return normalize(
                    transpose(
                      inverse(
                        /*mat3(aViewMat) */ mat3(aModelMat)
                      )
                    )
                  * vec3(aNormal));
}

// ======================
// CalculateWorldPosition:
// Calculates the position used for this vertex in world space
// takes the model matrix, animation matrix, and the local position value to calculate with
vec3 CalculateWorldPosition(mat4 aModelMat, vec4 aPosition)
{
  return vec3(aModelMat * aPosition);
}

// ======================
// CalculateViewPosition:
// Calculates the position used for this vertex in view space
// takes the view matrix and the world position value to calculate with
vec4 CalculateViewPosition(mat4 aViewMat, vec4 aPosition)
{
  return aViewMat * aPosition;
}


// ======================
// InfluenceMapping:
// Finds the color and positional influences of the world
InfluenceMappedData InfluenceMapping(vec3 position)
{
  InfluenceMappedData imd;
  imd.mPos = position;
  imd.mColor = vec3(1,1,1);

  vec3 ColorInfluence = vec3(0,0,0);
  float HeightInfluence = 1.0f;

  for (int i = 0; i < WaterInfo.mNumberOfInfluences; ++i)
  {
    WaterInformation map = WaterInfo.mInfluenceMaps[i];
    vec2 vertToCenter = imd.mPos.xz - map.mCenter.xz;
    float vtcLen = length(vertToCenter);

    if (map.mActive > 0)
    {
      if (vtcLen <= map.mRadius)
      {
        // influence 
        // converts the length to a value of 0 being the center, and 1 being the radius distance from the center
        float influenceAmount = vtcLen / map.mRadius;

        // color
        if (map.mColorIntensity >= 0.0f)
        {
          float colInf = 1.0f - influenceAmount;

          if (InfluenceType_Linear == map.mColorInfluenceFunction)
          {
            ColorInfluence = mix(ColorInfluence, map.mColor * map.mColorIntensity, colInf);
          }
          else if (InfluenceType_Squared == map.mColorInfluenceFunction)
          {
            ColorInfluence = mix(ColorInfluence, map.mColor * map.mColorIntensity, colInf * colInf);
          }
          else if (InfluenceType_Cubic == map.mColorInfluenceFunction)
          {
            ColorInfluence = mix(ColorInfluence, map.mColor * map.mColorIntensity, colInf * colInf * colInf);
          }
          else if (InfluenceType_Logarithmic == map.mColorInfluenceFunction)
          {
            ColorInfluence = mix(ColorInfluence, map.mColor * map.mColorIntensity, log2(colInf));
          }
          //ColorInfluence = mix(ColorInfluence, map.mColor * map.mColorIntensity, 1.0f - influenceAmount);
        }

        // wave height
        if (map.mWaveIntensity >= 0.0f)
        {
          // 0 will make the height be the base height
          // 1 will not change the height
          if (InfluenceType_Linear == map.mWaveInfluenceFunction)
          {
            HeightInfluence *= (influenceAmount * (1.0f - map.mWaveIntensity));
          }
          else if (InfluenceType_Squared == map.mWaveInfluenceFunction)
          {
            HeightInfluence *= ((influenceAmount * influenceAmount) * (1.0f - map.mWaveIntensity));
          }
          else if (InfluenceType_Cubic == map.mWaveInfluenceFunction)
          {
            HeightInfluence *= ((influenceAmount * influenceAmount * influenceAmount) * (1.0f - map.mWaveIntensity));
          }
          else if (InfluenceType_Logarithmic == map.mWaveInfluenceFunction)
          {
            HeightInfluence *= (log2(influenceAmount) * (1.0f - map.mWaveIntensity));
          }
          //HeightInfluence *= (influenceAmount * (1.0f - map.mWaveIntensity));
        }
      }
    }
  }

  float yPos = imd.mPos.y;
  yPos -= WaterInfo.mBaseHeight;
  yPos *= HeightInfluence;
  yPos += WaterInfo.mBaseHeight;
  imd.mPos.y = yPos;

  imd.mColor *= ColorInfluence;

  return imd;
}




// ======================
// Main:
// Entry point of shader
void main() 
{
  // remaining output for fragment shader
  outTextureCoordinates = inTextureCoordinates.xy;
  outViewMatrix = View.mViewMatrix;

  outPositionWorld = CalculateWorldPosition(Model.mModelMatrix, vec4(inPosition, 1.0f));

  InfluenceMappedData imd = InfluenceMapping(outPositionWorld);
  outPositionWorld = imd.mPos;
  outColor = imd.mColor;

  outPosition = CalculateViewPosition(View.mViewMatrix, vec4(imd.mPos, 1.0f));

  outNormal = CalculateNormal(View.mViewMatrix,
                              Model.mModelMatrix,
                              vec4(inNormal, 0.0f));

  CalculatePosition(View.mProjectionMatrix,
                    outPosition);
}
