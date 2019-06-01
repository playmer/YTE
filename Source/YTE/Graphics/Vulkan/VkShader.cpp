#include <tuple>

#include "fmt/format.h"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Graphics/Vulkan/VkShaderCompiler.hpp"
#include "YTE/Graphics/Vulkan/VkShaderDescriptions.hpp"

namespace YTE
{
  YTEDefineType(VkShader)
  {
    RegisterType<VkShader>();
    TypeBuilder<VkShader> builder;
  }

  VkShader::VkShader(VkCreatePipelineDataSet& aInfo, ViewData* aView)
    : Shader(aInfo.mName)
    , mSurface(aInfo.mSurface)
    , mView(aView)
  {
    Load(aInfo);
  }

  VkShader::~VkShader()
  {
  }

  VkCreatePipelineDataSet VkShader::CreateInfo(
    std::string const& aName,
    VkRenderedSurface* aSurface,
    std::shared_ptr<vkhlf::DescriptorSetLayout> aDescriptorSetLayout,
    std::shared_ptr<vkhlf::PipelineLayout> aLayout,
    VkShaderDescriptions &aDescriptions,
    bool aReload)
  {
    UnusedArguments(aReload);
    auto device = aSurface->GetDevice();

    auto lines = aDescriptions.GetLines();

    // Current Shader types supported. [file type, vk stage, required stage or not]
    constexpr std::array types = { 
      std::tuple(".comp", vk::ShaderStageFlagBits::eCompute, false),
      std::tuple(".vert", vk::ShaderStageFlagBits::eVertex, true),
      std::tuple(".frag", vk::ShaderStageFlagBits::eFragment, true)
    };

    std::vector<SpirV> shaderSpriv;

    bool hadErrors = false;

    // Find and compile all shaders in the shader set.
    for (auto const& [type, stage, required] : types)
    {
      auto fileName = aName + type;
      auto file = Path::GetShaderPath(Path::GetEnginePath(), fileName.c_str());

      auto fileExists = std::filesystem::exists(file);

      if ((false == fileExists) && required)
      {
        auto engine = aSurface->GetRenderer()->GetEngine();
        auto errStr = fmt::format("Could not find the {} file: {}", type, file);
        engine->Log(LogType::Information, errStr);

        VkCreatePipelineDataSet ret(aName, errStr);
        return ret;
      }
      else if (false == fileExists)
      {
        continue;
      }

      shaderSpriv.emplace_back(CompileGLSLToSPIRV(stage, file, lines, false));

      hadErrors = hadErrors || (false == shaderSpriv.back().mValid);
    }

    // Handle errors from miscompiled shaders we found: 
    if (hadErrors)
    {
      auto errorText = fmt::format("The ShaderSet named {} had the following shader compilation errors:", aName);

      for (auto const& spriv : shaderSpriv)
      {
        auto stage = to_string(spriv.mStage);
        errorText.append(fmt::format("\n{} Stage: {}", stage, spriv.mReason));
      }
      errorText.append("\n");

      aSurface->GetRenderer()->GetEngine()->Log(LogType::Error, errorText);
    
      VkCreatePipelineDataSet ret(aName, errorText);
      return ret;
    }


    std::vector<vkhlf::PipelineShaderStageCreateInfo> pipelineStageInfos;

    // Make shader modules
    for (auto const& spriv : shaderSpriv)
    {
      auto module = device->createShaderModule(spriv.mData);

      pipelineStageInfos.emplace_back(spriv.mStage, module, "main");
    }


    // Initialize Pipeline
    std::shared_ptr<vkhlf::PipelineCache> pipelineCache = device->createPipelineCache(0, nullptr);

    vkhlf::PipelineVertexInputStateCreateInfo vertexInput(aDescriptions.Bindings(),
                                                          aDescriptions.Attributes());
    vk::PipelineInputAssemblyStateCreateInfo assembly({},
                                                      vk::PrimitiveTopology::eTriangleList,
                                                      VK_FALSE);

    // One dummy viewport and scissor, as dynamic state sets them.
    vkhlf::PipelineViewportStateCreateInfo viewport({ {} }, { {} });
    vk::PipelineRasterizationStateCreateInfo rasterizationNoCull(
      {},
      false,
      false,
      vk::PolygonMode::eFill,
      vk::CullModeFlagBits::eNone,
      vk::FrontFace::eCounterClockwise,
      false,
      0.0f,
      0.0f,
      0.0f,
      1.0f);

    vk::PipelineRasterizationStateCreateInfo rasterizationCullBack(
      {},
      false,
      false,
      vk::PolygonMode::eFill,
      vk::CullModeFlagBits::eBack,
      vk::FrontFace::eCounterClockwise,
      false,
      0.0f,
      0.0f,
      0.0f,
      1.0f);

    vkhlf::PipelineMultisampleStateCreateInfo multisample(
      vk::SampleCountFlagBits::e1,
      false,
      0.0f,
      nullptr,
      false,
      false);

    vk::StencilOpState stencilOpState(
      vk::StencilOp::eKeep,
      vk::StencilOp::eKeep,
      vk::StencilOp::eKeep,
      vk::CompareOp::eAlways,
      0,
      0,
      0);

    vk::PipelineDepthStencilStateCreateInfo enableDepthStencil(
      {},
      true,
      true,
      vk::CompareOp::eLessOrEqual,
      false,
      false,
      stencilOpState,
      stencilOpState,
      0.0f,
      0.0f);

    vk::PipelineDepthStencilStateCreateInfo disableDepthStencil(
      {},
      true,
      true,
      vk::CompareOp::eLessOrEqual,
      false,
      false,
      stencilOpState,
      stencilOpState,
      0.0f,
      0.0f);

    vk::PipelineColorBlendAttachmentState noColorBlendAttachment(
      false,                                 // enableBlend
      vk::BlendFactor::eSrcColor,            // SrcColorBlendFactor
      vk::BlendFactor::eOne,                 // DstColorBlendFactor
      vk::BlendOp::eAdd,                     // ColorBlendOp
      vk::BlendFactor::eOne,                 // SrcAlphaBlendFactor
      vk::BlendFactor::eOneMinusSrcAlpha,    // DstAlphaBlendFactor
      vk::BlendOp::eAdd,                     // AlphaBlendOp
      vk::ColorComponentFlagBits::eR |       // ColorWriteMask
      vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB |
      vk::ColorComponentFlagBits::eA);

    vk::PipelineColorBlendAttachmentState additiveColorBlendAttachment(
      true,                                  // enableBlend
      vk::BlendFactor::eOne,                 // SrcColorBlendFactor
      vk::BlendFactor::eOne,                 // DstColorBlendFactor
      vk::BlendOp::eAdd,                     // ColorBlendOp
      vk::BlendFactor::eOne,                 // SrcAlphaBlendFactor
      vk::BlendFactor::eOne,                 // DstAlphaBlendFactor
      vk::BlendOp::eAdd,                     // AlphaBlendOp
      vk::ColorComponentFlagBits::eR |       // ColorWriteMask
      vk::ColorComponentFlagBits::eG |       
      vk::ColorComponentFlagBits::eB |       
      vk::ColorComponentFlagBits::eA);
    
    vk::PipelineColorBlendAttachmentState alphaColorBlendAttachment(
      true,                               // enableBlend
      vk::BlendFactor::eSrcAlpha,         // SrcColorBlendFactor
      vk::BlendFactor::eOneMinusSrcAlpha, // DstColorBlendFactor
      vk::BlendOp::eAdd,                  // ColorBlendOp
      vk::BlendFactor::eSrcAlpha,         // SrcAlphaBlendFactor
      vk::BlendFactor::eOneMinusSrcAlpha, // DstAlphaBlendFactor
      vk::BlendOp::eAdd,                  // AlphaBlendOp
      vk::ColorComponentFlagBits::eR |    // ColorWriteMask
      vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB |
      vk::ColorComponentFlagBits::eA);

    vkhlf::PipelineColorBlendStateCreateInfo noColorBlend(false,
                                                          vk::LogicOp::eNoOp,
                                                          noColorBlendAttachment,
                                                          { 1.0f, 1.0f, 1.0f, 1.0f });
    vkhlf::PipelineColorBlendStateCreateInfo additiveColorBlend(false,
                                                                vk::LogicOp::eNoOp,
                                                                additiveColorBlendAttachment,
                                                                { 1.0f, 1.0f, 1.0f, 1.0f });
    vkhlf::PipelineColorBlendStateCreateInfo alphaColorBlend(false,
                                                             vk::LogicOp::eClear,
                                                             alphaColorBlendAttachment,
                                                             { 0.0f, 0.0f, 0.0f, 0.0f });

    vkhlf::PipelineDynamicStateCreateInfo dynamic({ vk::DynamicState::eViewport,
                                                    vk::DynamicState::eScissor,
                                                    vk::DynamicState::eLineWidth});

    return VkCreatePipelineDataSet(
      pipelineCache,
      {},
      pipelineStageInfos,
      vertexInput,
      assembly,
      nullptr,
      viewport,
      rasterizationNoCull,
      rasterizationCullBack,
      multisample,
      enableDepthStencil,
      disableDepthStencil,
      noColorBlend,
      alphaColorBlend,
      additiveColorBlend,
      dynamic, 
      aDescriptorSetLayout,
      aLayout,
      aName,
      "",
      aSurface,
      aDescriptions,
      true);
  }


  void VkShader::Load(VkCreatePipelineDataSet& aInfo)
  {
    auto &renderPass = mView->mRenderTarget->GetRenderPass();

    mTriangles = mSurface->GetDevice()->createGraphicsPipeline(
      aInfo.mPipelineCache,
      aInfo.mFlags,
      aInfo.mStages,
      aInfo.mVertexInput,
      aInfo.mAssembly,
      aInfo.mTessalationState,
      aInfo.mViewport,
      aInfo.mRasterizationCullBack,
      aInfo.mMultiSample,
      aInfo.mEnableDepthStencil,
      aInfo.mNoColorBlend,
      aInfo.mDynamicState,
      aInfo.mPipelineLayout,
      renderPass);

    aInfo.mRasterizationNoCull.setPolygonMode(vk::PolygonMode::eLine);
    mWireframe = mSurface->GetDevice()->createGraphicsPipeline(
      aInfo.mPipelineCache,
      aInfo.mFlags,
      aInfo.mStages,
      aInfo.mVertexInput,
      aInfo.mAssembly,
      aInfo.mTessalationState,
      aInfo.mViewport,
      aInfo.mRasterizationNoCull,
      aInfo.mMultiSample,
      aInfo.mEnableDepthStencil,
      aInfo.mNoColorBlend,
      aInfo.mDynamicState,
      aInfo.mPipelineLayout,
      renderPass);

    aInfo.mRasterizationNoCull.setPolygonMode(vk::PolygonMode::eFill);

    aInfo.mAssembly.setTopology(vk::PrimitiveTopology::eLineList);

    mLines = mSurface->GetDevice()->createGraphicsPipeline(
      aInfo.mPipelineCache,
      aInfo.mFlags,
      aInfo.mStages,
      aInfo.mVertexInput,
      aInfo.mAssembly,
      aInfo.mTessalationState,
      aInfo.mViewport,
      aInfo.mRasterizationNoCull,
      aInfo.mMultiSample,
      aInfo.mEnableDepthStencil,
      aInfo.mNoColorBlend,
      aInfo.mDynamicState,
      aInfo.mPipelineLayout,
      renderPass);

    aInfo.mAssembly.setTopology(vk::PrimitiveTopology::eLineStrip);

    mCurves = mSurface->GetDevice()->createGraphicsPipeline(
      aInfo.mPipelineCache,
      aInfo.mFlags,
      aInfo.mStages,
      aInfo.mVertexInput,
      aInfo.mAssembly,
      aInfo.mTessalationState,
      aInfo.mViewport,
      aInfo.mRasterizationNoCull,
      aInfo.mMultiSample,
      aInfo.mEnableDepthStencil,
      aInfo.mNoColorBlend,
      aInfo.mDynamicState,
      aInfo.mPipelineLayout,
      renderPass);

    aInfo.mAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

    mShaderNoCull = mSurface->GetDevice()->createGraphicsPipeline(
      aInfo.mPipelineCache,
      aInfo.mFlags,
      aInfo.mStages,
      aInfo.mVertexInput,
      aInfo.mAssembly,
      aInfo.mTessalationState,
      aInfo.mViewport,
      aInfo.mRasterizationNoCull,
      aInfo.mMultiSample,
      aInfo.mEnableDepthStencil,
      aInfo.mNoColorBlend,
      aInfo.mDynamicState,
      aInfo.mPipelineLayout,
      renderPass);
                       

    mAlphaBlendShader = mSurface->GetDevice()->createGraphicsPipeline(
      aInfo.mPipelineCache,
      aInfo.mFlags,
      aInfo.mStages,
      aInfo.mVertexInput,
      aInfo.mAssembly,
      aInfo.mTessalationState,
      aInfo.mViewport,
      aInfo.mRasterizationNoCull,
      aInfo.mMultiSample,
      aInfo.mEnableDepthStencil,
      aInfo.mAlphaColorBlend,
      aInfo.mDynamicState,
      aInfo.mPipelineLayout,
      renderPass);

    mAdditiveBlendShader = mSurface->GetDevice()->createGraphicsPipeline(
      aInfo.mPipelineCache,
      aInfo.mFlags,
      aInfo.mStages,
      aInfo.mVertexInput,
      aInfo.mAssembly,
      aInfo.mTessalationState,
      aInfo.mViewport,
      aInfo.mRasterizationNoCull,
      aInfo.mMultiSample,
      aInfo.mDisableDepthStencil,
      aInfo.mAdditiveColorBlend,
      aInfo.mDynamicState,
      aInfo.mPipelineLayout,
      renderPass);
  }


  void VkShader::Reload(VkCreatePipelineDataSet& aInfo)
  {
    Load(aInfo);
  }
}
