#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


///////////////////////////////////////////////////////////////////////////////
// Defines
#define M_PI 3.1415926535897932384626433832795

///////////////////////////////////////////////////////////////////////////////
// UBO Buffers

// ========================
// Material Values
layout (binding = 2) uniform UBOMaterial
{
    vec4 mDiffuse;
    vec4 mAmbient;
    vec4 mSpecular;
    vec4 mEmissive;
    vec4 mTransparent;
    vec4 mReflective;
    float mOpacity;
    float mShininess;
    float mShininessStrength;
    float mReflectivity;
    float mReflectiveIndex;
    float mBumpScaling;
    vec2 mPadding;    // not used
} Material;


///////////////////////////////////////////////////////////////////////////////
// Samplers
layout (binding = 3) uniform sampler2D environmentMap;


///////////////////////////////////////////////////////////////////////////////
// Fragment Shader Inputs | Vertex Shader Outputs
layout (location = 0) in vec3 inTextureCoordinates;
layout (location = 1) in vec3 inEyeVector;


// ========================
// Output of Fragment
layout (location = 0) out vec4 outFragColor;
layout (location = 0) out vec4 outFragColor;


// ======================
// Main:
// Entry Point of Shader
void main()
{
  vec3 V = normalize(inEyeVector);

  float u = atan(V.z,V.x) / (M_PI * 2) + 0.5;
  float v = 1.0 - acos(V.y) / M_PI;

  vec2 skyUv = vec2(u, v);

  outFragColor = texture(environmentMap, skyUv);
}