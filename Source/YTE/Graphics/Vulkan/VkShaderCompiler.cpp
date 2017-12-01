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

  SpirV CompileGLSLToSPIRV(vk::ShaderStageFlagBits stage, 
                           std::string const &aFile,
                           std::string &aDefines)
  {
    static StaticGLSLCompiler compiler;

    std::string shaderText;
    ReadFileToString(aFile, shaderText);

    TBuiltInResource  resource;

    resource.maxLights = 32;
    resource.maxClipPlanes = 6;
    resource.maxTextureUnits = 32;
    resource.maxTextureCoords = 32;
    resource.maxVertexAttribs = 64;
    resource.maxVertexUniformComponents = 4096;
    resource.maxVaryingFloats = 64;
    resource.maxVertexTextureImageUnits = 32;
    resource.maxCombinedTextureImageUnits = 80;
    resource.maxTextureImageUnits = 32;
    resource.maxFragmentUniformComponents = 4096;
    resource.maxDrawBuffers = 32;
    resource.maxVertexUniformVectors = 128;
    resource.maxVaryingVectors = 8;
    resource.maxFragmentUniformVectors = 16;
    resource.maxVertexOutputVectors = 16;
    resource.maxFragmentInputVectors = 15;
    resource.minProgramTexelOffset = -8;
    resource.maxProgramTexelOffset = 7;
    resource.maxClipDistances = 8;
    resource.maxComputeWorkGroupCountX = 65535;
    resource.maxComputeWorkGroupCountY = 65535;
    resource.maxComputeWorkGroupCountZ = 65535;
    resource.maxComputeWorkGroupSizeX = 1024;
    resource.maxComputeWorkGroupSizeY = 1024;
    resource.maxComputeWorkGroupSizeZ = 64;
    resource.maxComputeUniformComponents = 1024;
    resource.maxComputeTextureImageUnits = 16;
    resource.maxComputeImageUniforms = 8;
    resource.maxComputeAtomicCounters = 8;
    resource.maxComputeAtomicCounterBuffers = 1;
    resource.maxVaryingComponents = 60;
    resource.maxVertexOutputComponents = 64;
    resource.maxGeometryInputComponents = 64;
    resource.maxGeometryOutputComponents = 128;
    resource.maxFragmentInputComponents = 128;
    resource.maxImageUnits = 8;
    resource.maxCombinedImageUnitsAndFragmentOutputs = 8;
    resource.maxCombinedShaderOutputResources = 8;
    resource.maxImageSamples = 0;
    resource.maxVertexImageUniforms = 0;
    resource.maxTessControlImageUniforms = 0;
    resource.maxTessEvaluationImageUniforms = 0;
    resource.maxGeometryImageUniforms = 0;
    resource.maxFragmentImageUniforms = 8;
    resource.maxCombinedImageUniforms = 8;
    resource.maxGeometryTextureImageUnits = 16;
    resource.maxGeometryOutputVertices = 256;
    resource.maxGeometryTotalOutputComponents = 1024;
    resource.maxGeometryUniformComponents = 1024;
    resource.maxGeometryVaryingComponents = 64;
    resource.maxTessControlInputComponents = 128;
    resource.maxTessControlOutputComponents = 128;
    resource.maxTessControlTextureImageUnits = 16;
    resource.maxTessControlUniformComponents = 1024;
    resource.maxTessControlTotalOutputComponents = 4096;
    resource.maxTessEvaluationInputComponents = 128;
    resource.maxTessEvaluationOutputComponents = 128;
    resource.maxTessEvaluationTextureImageUnits = 16;
    resource.maxTessEvaluationUniformComponents = 1024;
    resource.maxTessPatchComponents = 120;
    resource.maxPatchVertices = 32;
    resource.maxTessGenLevel = 64;
    resource.maxViewports = 16;
    resource.maxVertexAtomicCounters = 0;
    resource.maxTessControlAtomicCounters = 0;
    resource.maxTessEvaluationAtomicCounters = 0;
    resource.maxGeometryAtomicCounters = 0;
    resource.maxFragmentAtomicCounters = 8;
    resource.maxCombinedAtomicCounters = 8;
    resource.maxAtomicCounterBindings = 1;
    resource.maxVertexAtomicCounterBuffers = 0;
    resource.maxTessControlAtomicCounterBuffers = 0;
    resource.maxTessEvaluationAtomicCounterBuffers = 0;
    resource.maxGeometryAtomicCounterBuffers = 0;
    resource.maxFragmentAtomicCounterBuffers = 1;
    resource.maxCombinedAtomicCounterBuffers = 1;
    resource.maxAtomicCounterBufferSize = 16384;
    resource.maxTransformFeedbackBuffers = 4;
    resource.maxTransformFeedbackInterleavedComponents = 64;
    resource.maxCullDistances = 8;
    resource.maxCombinedClipAndCullDistances = 8;
    resource.maxSamples = 4;
    resource.limits.nonInductiveForLoops = 1;
    resource.limits.whileLoops = 1;
    resource.limits.doWhileLoops = 1;
    resource.limits.generalUniformIndexing = 1;
    resource.limits.generalAttributeMatrixVectorIndexing = 1;
    resource.limits.generalVaryingIndexing = 1;
    resource.limits.generalSamplerIndexing = 1;
    resource.limits.generalVariableIndexing = 1;
    resource.limits.generalConstantMatrixVectorIndexing = 1;

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
