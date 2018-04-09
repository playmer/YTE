#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

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
// Instancing Issues
#ifdef INSTANCING
  layout (location = 11) in vec4 inMatrix0;
  layout (location = 12) in vec4 inMatrix1;
  layout (location = 13) in vec4 inMatrix2;
  layout (location = 14) in vec4 inMatrix3;

  struct 
  {
    mat4 mModelMatrix;
  } Model;

  Model.mModelMatrix[0] = inMatrix1;
  Model.mModelMatrix[1] = inMatrix2;
  Model.mModelMatrix[2] = inMatrix3;
  Model.mModelMatrix[3] = inMatrix4;
#else
  // ========================
  // Model Matrix Buffer
  layout (binding = UBO_MODEL_BINDING) uniform UBOModel
  {
    mat4 mModelMatrix;
    vec4 mDiffuseColor;
  } Model;
#endif


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

///////////////////////////////////////////////////////////////////////////////
// Vertex Shader Outputs | Fragment Shader Inputs
layout (location = 0) out vec4 outDiffuse;
layout (location = 1) out vec2 outTextureCoordinates;

// ========================
// Positional Output of Vertex
out gl_PerVertex 
{
    vec4 gl_Position;
};

// ======================
// Main:
// Entry point of shader
void main() 
{
  outTextureCoordinates = vec2(inTextureCoordinates.x, inTextureCoordinates.y);
  outDiffuse = inColor;
  
  // TODO: Make scale actually use the window size.
  vec2 scale = vec2(2.0f / 1080, 2.0f / 1920);
  vec2 translate = vec2(-1.f, 1.f);
  gl_Position = Model.mModelMatrix     *
                vec4(inPosition, 1.0f);
}