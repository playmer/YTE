///////////////////////////////////////////////////////////////////////////////
// Defines
#define MAX_BONES 64


///////////////////////////////////////////////////////////////////////////////
// Vertex Layouts
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;


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
} Animation;

///////////////////////////////////////////////////////////////////////////////
// Vertex Shader Outputs | Fragment Shader Inputs
layout (location = 0) out vec3 outTextureCoordinates;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outPosition;
layout (location = 3) out vec3 outPositionWorld;
layout (location = 4) out mat4 outViewMatrix; // 6 - 9


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
// Main:
// Entry point of shader
void main() 
{
  // remaining output for fragment shader
  outTextureCoordinates = vec3(inTextureCoordinates.x, 1.0 - inTextureCoordinates.y, inTextureCoordinates.z);
  outViewMatrix = View.mViewMatrix;

  outPositionWorld = CalculateWorldPosition(Model.mModelMatrix, vec4(inPosition, 1.0f));
  outPosition = CalculateViewPosition(View.mViewMatrix, vec4(outPositionWorld, 1.0f));
  outNormal = CalculateNormal(Model.mModelMatrix, inNormal);

  gl_Position = View.mProjectionMatrix * outPosition;
}