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
layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTextureCoordinates;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec4 outPosition;
layout (location = 4) out vec3 outPositionWorld;
layout (location = 5) out vec4 outDiffuse;
layout (location = 6) out mat4 outViewMatrix; // 6 - 9


// ========================
// Positional Output of Vertex
out gl_PerVertex 
{
    vec4 gl_Position;
};








///////////////////////////////////////////////////////////////////////////////
// Functions
// ======================
// CalculateNormal:
// Calculates the normal used for this vertex
// takes the view matrix, model matrix, animation matrix, and the normal value to calculate with
vec3 CalculateNormal(mat4 aModelMat, vec3 aNormal)
{
  return normalize(mat3(transpose(inverse(aModelMat))) * aNormal);
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
// Main:
// Entry point of shader
void main() 
{
  // remaining output for fragment shader
  outColor = inColor.xyz;
  outTextureCoordinates = vec2(inTextureCoordinates.x, 1.0 - inTextureCoordinates.y);
  outDiffuse = Model.mDiffuseColor;
  outViewMatrix = View.mViewMatrix;

  outPositionWorld = CalculateWorldPosition(Model.mModelMatrix, vec4(inPosition, 1.0f));
  outPosition = CalculateViewPosition(View.mViewMatrix, vec4(outPositionWorld, 1.0f));
  outNormal = CalculateNormal(Model.mModelMatrix,
                              inNormal);
  CalculatePosition(View.mProjectionMatrix,
                    outPosition);

  // Unsure if this needs to be vec3/mat3 here.
  //vec3 position = vec3(mat3(View.mViewMatrix * Model.mModelMatrix) * inPosition);
  //gl_Position = vec4(View.mProjectionMatrix * vec4(position, 1.0));

  //gl_Position = View.mProjectionMatrix * 
  //              View.mViewMatrix       *
  //              Model.mModelMatrix     *
  //              vec4(inPosition, 1.0f);


  // Vulkan Specific Coordinate System Fix (fixes the depth of the vertex)
  //gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;  
}