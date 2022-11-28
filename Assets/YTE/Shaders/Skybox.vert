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
layout (location = 0) out vec3 outTextureCoordinates;
layout (location = 1) out vec3 outEyeVector;
layout (location = 2) out vec4 outDiffuse;

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
  mat4 modelMatrix = Model.mModelMatrix;
  modelMatrix[3].x = 0.0f;
  modelMatrix[3].y = 0.0f;
  modelMatrix[3].z = 0.0f;

  vec4 worldPosition = (modelMatrix * vec4(inPosition, 1.0f));

  modelMatrix[3].x = View.mCameraPosition.x;
  modelMatrix[3].y = View.mCameraPosition.y;
  modelMatrix[3].z = View.mCameraPosition.z;

  outEyeVector = worldPosition.xyz;
  outDiffuse = Model.mDiffuseColor;

  outTextureCoordinates = inTextureCoordinates;

  gl_Position = View.mProjectionMatrix * 
                View.mViewMatrix       *
                modelMatrix            *
                vec4(inPosition, 1.0f);

  // Vulkan Specific Coordinate System Fix (fixes the depth of the vertex)
  //gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;  
}