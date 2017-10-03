#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec3 inTangent;
layout (location = 5) in vec3 inBinormal;
layout (location = 6) in vec3 inBitangent;

layout (binding = 0) uniform UBOView
{
  mat4 mProjectionMatrix;
  mat4 mViewMatrix;
} View;

layout (binding = 1) uniform UBOModel
{
  mat4 mModelMatrix;
} Model;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTextureCoordinates;
layout (location = 2) out vec3 outNormal;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
  outColor = inColor;
  outTextureCoordinates = inTextureCoordinates.xy;
  outNormal = vec3(inverse(transpose(View.mViewMatrix * Model.mModelMatrix)) * vec4(inNormal, 1.0f));
  
  gl_Position = View.mProjectionMatrix * 
                View.mViewMatrix       *
                Model.mModelMatrix     *
                vec4(inPosition, 1.0f);
}