#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout(constant_id = 0) const int NumberOfTextures = 1;

layout (binding = 1) uniform sampler2D textureSampler[NumberOfTextures];

layout (location = 0) in vec2 inUVCoordinates;
layout (location = 2) in flat uint inTextureId;

layout (location = 0) out vec4 outFragColor;

void main() 
{
  vec4 textureColor = texture(textureSampler[inTextureId], inUVCoordinates);
  
  outFragColor = texture(textureSampler[inTextureId], inUVCoordinates);
}
