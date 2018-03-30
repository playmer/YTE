#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


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
// Constants
vec3 vertices[4] = { vec3( 1.0f,  1.0f, 0.0f),    // bottom right (was 1,1)
                     vec3(-1.0f,  1.0f, 0.0f),    // bottom left  (was 0,1)
                     vec3(-1.0f, -1.0f, 0.0f),    // top left     (was 0,0)
                     vec3( 1.0f, -1.0f, 0.0f) };  // top right    (was 1,0)



///////////////////////////////////////////////////////////////////////////////
// UBO Buffers



///////////////////////////////////////////////////////////////////////////////
// Vertex Shader Outputs | Fragment Shader Inputs
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outTextureCoordinates;


// ========================
// Positional Output of Vertex
out gl_PerVertex 
{
    vec4 gl_Position;
};





///////////////////////////////////////////////////////////////////////////////
// Functions

// ======================
// Main:
// Entry point of shader
void main() 
{
  // remaining output for fragment shader
  outColor = vec4(inColor.xyz, 1.0f);
  outTextureCoordinates = inTextureCoordinates.xy;

  // Initial Position Update
  gl_Position = vec4(vertices[gl_VertexIndex], 1.0f);

  // Vulkan Specific Coordinate System Fix (fixes the depth of the vertex)
  gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;  

  // morph to screen
  //outTextureCoordinates = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	//gl_Position = vec4(outTextureCoordinates * 2.0f - 1.0f, 0.0f, 1.0f);
}
