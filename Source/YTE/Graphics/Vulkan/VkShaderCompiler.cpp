#include <map>

#include "VkHLF/3rdparty/glslang/SPIRV/GlslangToSpv.h"

#include "YTE/Graphics/Vulkan/VkShaderCompiler.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  static const TBuiltInResource cDefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,

    /* .limits = */ {
    /* .nonInductiveForLoops = */ 1,
    /* .whileLoops = */ 1,
    /* .doWhileLoops = */ 1,
    /* .generalUniformIndexing = */ 1,
    /* .generalAttributeMatrixVectorIndexing = */ 1,
    /* .generalVaryingIndexing = */ 1,
    /* .generalSamplerIndexing = */ 1,
    /* .generalVariableIndexing = */ 1,
    /* .generalConstantMatrixVectorIndexing = */ 1,
} };

  // We have to call this once per process for some reason.
  struct StaticGLSLCompiler
  {
    StaticGLSLCompiler()
    {
      #ifndef __ANDROID__
        glslang::InitializeProcess();
      #endif
    }
  };

  SpirV CompileGLSLToSPIRV(vk::ShaderStageFlagBits aStage,
                           std::string const &aFile,
                           std::string &aDefines,
                           bool aFilenameIsShaderText)
  {
    static StaticGLSLCompiler compiler;

    std::string shaderText{"#version 450\n\n"};

    if (aFilenameIsShaderText)
    {
      shaderText = aFile;
    }
    else
    {
      ReadFileToString(aFile, shaderText);
    }

    TBuiltInResource  resource = cDefaultTBuiltInResource;

    const std::map<vk::ShaderStageFlagBits, EShLanguage> stageToLanguageMap
    {
      { vk::ShaderStageFlagBits::eVertex, EShLangVertex },
      { vk::ShaderStageFlagBits::eTessellationControl, EShLangTessControl },
      { vk::ShaderStageFlagBits::eTessellationEvaluation, EShLangTessEvaluation },
      { vk::ShaderStageFlagBits::eGeometry, EShLangGeometry },
      { vk::ShaderStageFlagBits::eFragment, EShLangFragment },
      { vk::ShaderStageFlagBits::eCompute, EShLangCompute }
    };

    SpirV spirv;

    const auto stageIt = stageToLanguageMap.find(aStage);

    if (stageIt == stageToLanguageMap.end())
    {
      spirv.mReason = "Invalid value passed as vk::ShaderStageFlagBits.";
      return spirv;
    }

    glslang::TProgram program;

    glslang::TShader shader(stageIt->second);

    const char *shaderStrings = shaderText.c_str();
    shader.setStrings(&shaderStrings, 1);
    shader.setPreamble(aDefines.c_str());

    // Enable SPIR-V and Vulkan rules when parsing GLSL
    EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

    if (!shader.parse(&resource, 100, false, messages))
    {
      spirv.mReason = "GLSL compiling failed:";
      spirv.mReason += "Log: ";
      spirv.mReason += shader.getInfoLog();

      spirv.mReason += "Debug Log: ";
      spirv.mReason += shader.getInfoDebugLog();
      return spirv;
    }

    program.addShader(&shader);

    if (!program.link(messages))
    {
      spirv.mReason = "GLSL compiling failed:";
      spirv.mReason += "Log: ";
      spirv.mReason += shader.getInfoLog();

      spirv.mReason += "Debug Log: ";
      spirv.mReason += shader.getInfoDebugLog();
      return spirv;
    }

    std::vector<uint32_t> code;
    glslang::GlslangToSpv(*program.getIntermediate(stageIt->second), code);

    spirv.mValid = true;
    spirv.mData = std::move(code);
    spirv.mStage = aStage;

    return std::move(spirv);
  }
}
