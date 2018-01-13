#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 2) uniform UBOMaterial
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

layout (binding = 3) uniform sampler2D diffuseSampler;

layout (location = 0) in vec2 inTextureCoordinates;

layout (location = 0) out vec4 outFragColor;

void main()
{
  outFragColor = texture(diffuseSampler, inTextureCoordinates);
  //outFragColor = vec4(inTextureCoordinates.y, 0.0f, 0.0f, 1.0f);
}