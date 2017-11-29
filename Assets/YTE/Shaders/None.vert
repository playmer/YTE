#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_BONES 64

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec3 inTangent;
layout (location = 5) in vec3 inBinormal;
layout (location = 6) in vec3 inBitangent;
layout (location = 7) in vec3 inBoneWeights;
layout (location = 8) in vec2 inBoneWeights2;
layout (location = 9) in ivec3 inBoneIDs;
layout (location = 10) in ivec2 inBoneIDs2;

layout (binding = 0) uniform UBOView
{
  mat4 mProjectionMatrix;
  mat4 mViewMatrix;
} View;

layout (binding = 1) uniform UBOModel
{
  mat4 mModelMatrix;
} Model;

layout (binding = 2) uniform UBOAnimation
{
  mat4 mBones[MAX_BONES];
  bool mHasAnimations;
} Animation;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTextureCoordinates;
layout (location = 2) out vec3 outNormal;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
  // animation matrix calculation
  mat4 boneTransform = mat4(1.0f);
  if (Animation.mHasAnimations)
  {
    outColor = vec3(1.0f, 0.0, 0.0);

    boneTransform  = Animation.mBones[inBoneIDs[0]] * inBoneWeights[0];
    boneTransform += Animation.mBones[inBoneIDs[1]] * inBoneWeights[1];
    boneTransform += Animation.mBones[inBoneIDs[2]] * inBoneWeights[2];
    boneTransform += Animation.mBones[inBoneIDs2[0]] * inBoneWeights2[0];
    boneTransform += Animation.mBones[inBoneIDs2[1]] * inBoneWeights2[1];
  } 

  outColor = inColor;
  outTextureCoordinates = inTextureCoordinates.xy;
  outNormal = vec3(inverse(transpose(View.mViewMatrix               *
                                    Model.mModelMatrix              * 
                                    transpose(boneTransform)))      * 
                                    vec4(inNormal, 1.0f));
  
  gl_Position = View.mProjectionMatrix   * 
                View.mViewMatrix         *
                Model.mModelMatrix       *
                transpose(boneTransform) *
                vec4(inPosition, 1.0f);

  gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f; // coordninate system fix
}
