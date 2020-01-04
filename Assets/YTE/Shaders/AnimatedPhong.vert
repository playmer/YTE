///////////////////////////////////////////////////////////////////////////////
// Defines
#define MAX_BONES 64


///////////////////////////////////////////////////////////////////////////////
// Vertex Layouts
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 inColor;
layout (location = 4) in vec3 inTangent;
layout (location = 5) in vec3 inBinormal;
layout (location = 6) in vec3 inBitangent;
layout (location = 7) in vec3 inBoneWeights;
layout (location = 8) in vec2 inBoneWeights2;
layout (location = 9) in ivec3 inBoneIDs;
layout (location = 10) in ivec2 inBoneIDs2;


///////////////////////////////////////////////////////////////////////////////
// UBO Buffers

// ========================
// Model Matrix Buffer
layout (binding = UBO_MODEL_BINDING) uniform UBOModel
{
  mat4 mModelMatrix;
  vec4 mDiffuseColor;
} Model;

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
} Animation;



///////////////////////////////////////////////////////////////////////////////
// Vertex Shader Outputs | Fragment Shader Inputs
layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTextureCoordinates;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec4 outPosition;
layout (location = 4) out vec3 outPositionWorld;
layout (location = 5) out mat4 outViewMatrix; // 5 - 8

// ========================
// Positional Output of Vertex
out gl_PerVertex 
{
    vec4 gl_Position;
};



///////////////////////////////////////////////////////////////////////////////
// Functions


// ======================
// Animate:
// outputs a transform matrix for the shader to use with position of the vertex
mat4 Animate()
{
  mat4 boneTransform;
  boneTransform  = Animation.mBones[inBoneIDs[0]] * inBoneWeights[0];
  boneTransform += Animation.mBones[inBoneIDs[1]] * inBoneWeights[1];
  boneTransform += Animation.mBones[inBoneIDs[2]] * inBoneWeights[2];
  boneTransform += Animation.mBones[inBoneIDs2[0]] * inBoneWeights2[0];
  boneTransform += Animation.mBones[inBoneIDs2[1]] * inBoneWeights2[1];
  
  return boneTransform;
}

// ======================
// CalculateNormal:
// Calculates the normal used for this vertex
// takes the view matrix, model matrix, animation matrix, and the normal value to calculate with
vec3 CalculateNormal(mat4 aViewMat, mat4 aModelMat, mat4 aAnimateMat, vec3 aNormal)
{
  return normalize(mat3(transpose(inverse(/*aViewMat */ aModelMat /* * aAnimateMat*/))) * aNormal);
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
// Main:
// Entry point of shader
void main() 
{
  // animation matrix calculation
  mat4 boneTransform = Animate();

  // remaining output for fragment shader
  outColor = inColor.xyz;
  outTextureCoordinates = inTextureCoordinates.xy;

  outViewMatrix = View.mViewMatrix;

  outPositionWorld = vec3(Model.mModelMatrix * boneTransform * vec4(inPosition, 1.0f));
  //outPositionWorld = vec3(Model.mModelMatrix * vec4(inPosition, 1.0f));

  outPosition = CalculateViewPosition(View.mViewMatrix, vec4(outPositionWorld, 1.0f));

  outNormal = normalize(mat3(transpose(inverse(/* View.mViewMatrix * */ Model.mModelMatrix * boneTransform))) * inNormal);
  //outNormal = normalize(mat3(transpose(inverse(/* View.mViewMatrix * */ Model.mModelMatrix /* * boneTransform*/))) * inNormal);

  gl_Position = View.mProjectionMatrix * outPosition;
}