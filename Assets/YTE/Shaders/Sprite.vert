#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Vertex Data
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inUVCoordinates;
layout (location = 2) in vec3 inNormal;

// Instance Data
layout (location = 3) in uint inTextureId;
layout (location = 4) in vec4 inMatrix1;
layout (location = 5) in vec4 inMatrix2;
layout (location = 6) in vec4 inMatrix3;
layout (location = 7) in vec4 inMatrix4;


layout (binding = 0) uniform UBOView
{
  mat4 mProjectionMatrix;
  mat4 mViewMatrix;
} View;


layout (location = 0) out vec2 outUVCoordinates;
layout (location = 1) out flat uint outTextureId;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
  mat4 instanceMatrix = mat4(inMatrix1,
                             inMatrix2,
                             inMatrix3,
                             inMatrix4);

  outTextureId = inTextureId;
  outUVCoordinates = inUVCoordinates;

  gl_Position = View.mProjectionMatrix * 
                View.mViewMatrix       *
                instanceMatrix         *
                vec4(inPosition, 1.0f);
}