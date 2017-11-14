#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 3) uniform UBOMaterial
{
    vec3 mDiffuse;
    vec3 mAmbient;
    vec3 mSpecular;
    vec3 mEmissive;
    vec3 mTransparent;
    vec3 mReflective;
    float mOpacity;
    float mShininess;
    float mShininessStrength;
    float mReflectivity;
    float mReflectiveIndex;
    float mBumpScaling;
} Material;

layout (binding = 4) uniform sampler2D diffuseSampler;
layout (binding = 5) uniform sampler2D specularSampler;
layout (binding = 6) uniform sampler2D normalSampler;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTextureCoordinates;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;

void main()
{
  outFragColor = texture(specularSampler, inTextureCoordinates);
  outFragColor = texture(normalSampler, inTextureCoordinates);
  outFragColor = texture(diffuseSampler, inTextureCoordinates);

  //outFragColor = vec4(1,0,0, Material.mOpacity);
}
