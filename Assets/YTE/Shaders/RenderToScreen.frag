///////////////////////////////////////////////////////////////////////////////
// UBO Buffers



///////////////////////////////////////////////////////////////////////////////
// Samplers
layout (binding = UBO_RENDERTARGET_BINDING) uniform sampler2D renderTargetSampler;



///////////////////////////////////////////////////////////////////////////////
// Fragment Shader Inputs | Vertex Shader Outputs
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTextureCoordinates;

// ========================
// Output of Fragment
layout (location = 0) out vec4 outFragColor;


///////////////////////////////////////////////////////////////////////////////
// Functions

// ======================
// Saturate:
// Clamps a given value to the interval [0-1]
vec4 saturate(vec4 aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}
vec3 saturate(vec3 aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}
vec2 saturate(vec2 aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}
float saturate(float aValue)
{
  return clamp(aValue, 0.0f, 1.0f);
}



// ======================
// Main:
// Entry Point of Shader
void main()
{
  // fragment color
  outFragColor = mix(inColor, texture(renderTargetSampler, inTextureCoordinates.xy), 0.25f);
}
