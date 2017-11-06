#include <map>

#include "VkHLF/3rdparty/glslang/SPIRV/GlslangToSpv.h"

#include "YTE/Graphics/Vulkan/VkShaderCompiler.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
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

  SpirV CompileGLSLToSPIRV(vk::ShaderStageFlagBits stage, std::string const &aFile)
  {
    static StaticGLSLCompiler compiler;

    std::string shaderText;
    ReadFileToString(aFile, shaderText);

    TBuiltInResource  m_resource;

    m_resource.maxLights = 32;
    m_resource.maxClipPlanes = 6;
    m_resource.maxTextureUnits = 32;
    m_resource.maxTextureCoords = 32;
    m_resource.maxVertexAttribs = 64;
    m_resource.maxVertexUniformComponents = 4096;
    m_resource.maxVaryingFloats = 64;
    m_resource.maxVertexTextureImageUnits = 32;
    m_resource.maxCombinedTextureImageUnits = 80;
    m_resource.maxTextureImageUnits = 32;
    m_resource.maxFragmentUniformComponents = 4096;
    m_resource.maxDrawBuffers = 32;
    m_resource.maxVertexUniformVectors = 128;
    m_resource.maxVaryingVectors = 8;
    m_resource.maxFragmentUniformVectors = 16;
    m_resource.maxVertexOutputVectors = 16;
    m_resource.maxFragmentInputVectors = 15;
    m_resource.minProgramTexelOffset = -8;
    m_resource.maxProgramTexelOffset = 7;
    m_resource.maxClipDistances = 8;
    m_resource.maxComputeWorkGroupCountX = 65535;
    m_resource.maxComputeWorkGroupCountY = 65535;
    m_resource.maxComputeWorkGroupCountZ = 65535;
    m_resource.maxComputeWorkGroupSizeX = 1024;
    m_resource.maxComputeWorkGroupSizeY = 1024;
    m_resource.maxComputeWorkGroupSizeZ = 64;
    m_resource.maxComputeUniformComponents = 1024;
    m_resource.maxComputeTextureImageUnits = 16;
    m_resource.maxComputeImageUniforms = 8;
    m_resource.maxComputeAtomicCounters = 8;
    m_resource.maxComputeAtomicCounterBuffers = 1;
    m_resource.maxVaryingComponents = 60;
    m_resource.maxVertexOutputComponents = 64;
    m_resource.maxGeometryInputComponents = 64;
    m_resource.maxGeometryOutputComponents = 128;
    m_resource.maxFragmentInputComponents = 128;
    m_resource.maxImageUnits = 8;
    m_resource.maxCombinedImageUnitsAndFragmentOutputs = 8;
    m_resource.maxCombinedShaderOutputResources = 8;
    m_resource.maxImageSamples = 0;
    m_resource.maxVertexImageUniforms = 0;
    m_resource.maxTessControlImageUniforms = 0;
    m_resource.maxTessEvaluationImageUniforms = 0;
    m_resource.maxGeometryImageUniforms = 0;
    m_resource.maxFragmentImageUniforms = 8;
    m_resource.maxCombinedImageUniforms = 8;
    m_resource.maxGeometryTextureImageUnits = 16;
    m_resource.maxGeometryOutputVertices = 256;
    m_resource.maxGeometryTotalOutputComponents = 1024;
    m_resource.maxGeometryUniformComponents = 1024;
    m_resource.maxGeometryVaryingComponents = 64;
    m_resource.maxTessControlInputComponents = 128;
    m_resource.maxTessControlOutputComponents = 128;
    m_resource.maxTessControlTextureImageUnits = 16;
    m_resource.maxTessControlUniformComponents = 1024;
    m_resource.maxTessControlTotalOutputComponents = 4096;
    m_resource.maxTessEvaluationInputComponents = 128;
    m_resource.maxTessEvaluationOutputComponents = 128;
    m_resource.maxTessEvaluationTextureImageUnits = 16;
    m_resource.maxTessEvaluationUniformComponents = 1024;
    m_resource.maxTessPatchComponents = 120;
    m_resource.maxPatchVertices = 32;
    m_resource.maxTessGenLevel = 64;
    m_resource.maxViewports = 16;
    m_resource.maxVertexAtomicCounters = 0;
    m_resource.maxTessControlAtomicCounters = 0;
    m_resource.maxTessEvaluationAtomicCounters = 0;
    m_resource.maxGeometryAtomicCounters = 0;
    m_resource.maxFragmentAtomicCounters = 8;
    m_resource.maxCombinedAtomicCounters = 8;
    m_resource.maxAtomicCounterBindings = 1;
    m_resource.maxVertexAtomicCounterBuffers = 0;
    m_resource.maxTessControlAtomicCounterBuffers = 0;
    m_resource.maxTessEvaluationAtomicCounterBuffers = 0;
    m_resource.maxGeometryAtomicCounterBuffers = 0;
    m_resource.maxFragmentAtomicCounterBuffers = 1;
    m_resource.maxCombinedAtomicCounterBuffers = 1;
    m_resource.maxAtomicCounterBufferSize = 16384;
    m_resource.maxTransformFeedbackBuffers = 4;
    m_resource.maxTransformFeedbackInterleavedComponents = 64;
    m_resource.maxCullDistances = 8;
    m_resource.maxCombinedClipAndCullDistances = 8;
    m_resource.maxSamples = 4;
    m_resource.limits.nonInductiveForLoops = 1;
    m_resource.limits.whileLoops = 1;
    m_resource.limits.doWhileLoops = 1;
    m_resource.limits.generalUniformIndexing = 1;
    m_resource.limits.generalAttributeMatrixVectorIndexing = 1;
    m_resource.limits.generalVaryingIndexing = 1;
    m_resource.limits.generalSamplerIndexing = 1;
    m_resource.limits.generalVariableIndexing = 1;
    m_resource.limits.generalConstantMatrixVectorIndexing = 1;

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

    const auto stageIt = stageToLanguageMap.find(stage);

    if (stageIt == stageToLanguageMap.end())
    {
      spirv.mReason = "Invalid value passed as vk::ShaderStageFlagBits.";
      return spirv;
    }

    glslang::TShader shader(stageIt->second);

    const char *shaderStrings[1];
    shaderStrings[0] = shaderText.c_str();
    shader.setStrings(shaderStrings, 1);

    // Enable SPIR-V and Vulkan rules when parsing GLSL
    EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

    if (!shader.parse(&m_resource, 100, false, messages))
    {
      spirv.mReason = "GLSL compiling failed:";
      spirv.mReason += "Log: ";
      spirv.mReason += shader.getInfoLog();

      spirv.mReason += "Debug Log: ";
      spirv.mReason += shader.getInfoDebugLog();
      return spirv;
    }

    glslang::TProgram program;
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

    return std::move(spirv);
  }
}