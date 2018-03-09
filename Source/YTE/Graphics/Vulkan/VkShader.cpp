///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

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
    YTERegisterType(VkShader);
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


  VkCreatePipelineDataSet VkShader::CreateInfo(std::string &aName,
                                               VkRenderedSurface *aSurface,
                                               std::shared_ptr<vkhlf::PipelineLayout> aLayout,
                                               VkShaderDescriptions &aDescriptions,
                                               bool aReload)
  {
    auto device = aSurface->GetDevice();

    auto vertex = aName + ".vert";
    auto fragment = aName + ".frag";

    auto vertexFile = Path::GetShaderPath(Path::GetEnginePath(), vertex.c_str());
    auto fragmentFile = Path::GetShaderPath(Path::GetEnginePath(), fragment.c_str());


    auto lines = aDescriptions.GetLines();

    auto vertexData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eVertex, vertexFile, lines, false);
    auto fragmentData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eFragment, fragmentFile, lines, false);

    if (false == vertexData.mValid || false == fragmentData.mValid)
    {
      auto str = fmt::format("Vertex Shader named {}:\n {}\n-----------------\nFragment Shader named {}:\n {}", 
                             vertexFile, 
                             vertexData.mReason,
                             fragmentFile, 
                             fragmentData.mReason);

      //std::cout << str;
      aSurface->GetRenderer()->GetEngine()->Log(LogType::Error, fmt::format(
        "Shader: {} Failed to Load! Errors Follow:\n############################################################\n{}\n############################################################",
        aName,
        str));

      VkCreatePipelineDataSet ret(aName, str);
      return ret;
    }

    auto vertexModule = device->createShaderModule(vertexData.mData);
    auto fragmentModule = device->createShaderModule(fragmentData.mData);


    // Initialize Pipeline
    std::shared_ptr<vkhlf::PipelineCache> pipelineCache = device->createPipelineCache(0, nullptr);

    std::shared_ptr<vkhlf::PipelineShaderStageCreateInfo> vertexStage =
      std::make_shared<vkhlf::PipelineShaderStageCreateInfo>(vk::ShaderStageFlagBits::eVertex,
                                                             vertexModule,
                                                             "main");
    std::shared_ptr<vkhlf::PipelineShaderStageCreateInfo> fragmentStage =
      std::make_shared<vkhlf::PipelineShaderStageCreateInfo>(vk::ShaderStageFlagBits::eFragment,
                                                             fragmentModule,
                                                             "main");

    vkhlf::PipelineVertexInputStateCreateInfo vertexInput(aDescriptions.Bindings(),
                                                          aDescriptions.Attributes());
    vk::PipelineInputAssemblyStateCreateInfo assembly({},
                                                      vk::PrimitiveTopology::eTriangleList,
                                                      VK_FALSE);

    // One dummy viewport and scissor, as dynamic state sets them.
    vkhlf::PipelineViewportStateCreateInfo viewport({ {} }, { {} });
    vk::PipelineRasterizationStateCreateInfo rasterizationNoCull({},
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
    vk::PipelineRasterizationStateCreateInfo rasterizationCullBack({},
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

    vkhlf::PipelineMultisampleStateCreateInfo multisample(vk::SampleCountFlagBits::e1,
                                                          false,
                                                          0.0f,
                                                          nullptr,
                                                          false,
                                                          false);

    vk::StencilOpState stencilOpState(vk::StencilOp::eKeep,
                                      vk::StencilOp::eKeep,
                                      vk::StencilOp::eKeep,
                                      vk::CompareOp::eAlways,
                                      0,
                                      0,
                                      0);

    vk::PipelineDepthStencilStateCreateInfo enableDepthStencil({},
                                                               true,
                                                               true,
                                                               vk::CompareOp::eLessOrEqual,
                                                               false,
                                                               false,
                                                               stencilOpState,
                                                               stencilOpState,
                                                               0.0f,
                                                               0.0f);
    vk::PipelineDepthStencilStateCreateInfo disableDepthStencil({},
                                                                true,
                                                                true,
                                                                vk::CompareOp::eLessOrEqual,
                                                                false,
                                                                false,
                                                                stencilOpState,
                                                                stencilOpState,
                                                                0.0f,
                                                                0.0f);

    vk::PipelineColorBlendAttachmentState noColorBlendAttachment(false,                                 // enable
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
    vk::PipelineColorBlendAttachmentState additiveColorBlendAttachment(true,                                  // enable
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
    vk::PipelineColorBlendAttachmentState alphaColorBlendAttachment(true,                                  // enable
                                                                    vk::BlendFactor::eSrcAlpha,            // SrcColorBlendFactor
                                                                    vk::BlendFactor::eOneMinusSrcAlpha,    // DstColorBlendFactor
                                                                    vk::BlendOp::eAdd,                     // ColorBlendOp
                                                                    vk::BlendFactor::eOne,                 // SrcAlphaBlendFactor
                                                                    vk::BlendFactor::eZero,                // DstAlphaBlendFactor
                                                                    vk::BlendOp::eAdd,                     // AlphaBlendOp
                                                                    vk::ColorComponentFlagBits::eR |       // ColorWriteMask
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
                                                             vk::LogicOp::eNoOp,
                                                             alphaColorBlendAttachment,
                                                             { 1.0f, 1.0f, 1.0f, 1.0f });

    vkhlf::PipelineDynamicStateCreateInfo dynamic({ vk::DynamicState::eViewport,
                                                    vk::DynamicState::eScissor });

    if (aReload)
    {
      aSurface->GetRenderer()->GetEngine()->Log(LogType::Success, fmt::format("\t-> {} Reloaded Successfully!",
                                                                              aName));
    }
    else
    {
      aSurface->GetRenderer()->GetEngine()->Log(LogType::Success, fmt::format("Shader: {} Loaded Successfully!",
                                                                              aName));
    }

    return VkCreatePipelineDataSet(pipelineCache,
                                   {},
                                   vertexStage,
                                   fragmentStage,
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
                                   additiveColorBlend,
                                   dynamic,
                                   aLayout,
                                   aName,
                                   "",
                                   aSurface,
                                   aDescriptions,
                                   true);
  }


  void VkShader::Load(VkCreatePipelineDataSet& aInfo)
  {
    mShader = mSurface->GetDevice()->createGraphicsPipeline(aInfo.mPipelineCache,
                                                            aInfo.mFlags,
                                                            { *aInfo.mVertexStage.get(), *aInfo.mFragmentStage.get() },
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
                                                            mView->mRenderTarget->GetRenderPass());

    aInfo.mAssembly.setTopology(vk::PrimitiveTopology::eLineList);

    mShaderLines = mSurface->GetDevice()->createGraphicsPipeline(aInfo.mPipelineCache,
                                                                 aInfo.mFlags,
                                                                 { *aInfo.mVertexStage.get(), *aInfo.mFragmentStage.get() },
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
                                                                 mView->mRenderTarget->GetRenderPass());

    aInfo.mAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

    mShaderNoCull = mSurface->GetDevice()->createGraphicsPipeline(aInfo.mPipelineCache,
                                                                  aInfo.mFlags,
                                                                  { *aInfo.mVertexStage.get(), *aInfo.mFragmentStage.get() },
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
                                                                  mView->mRenderTarget->GetRenderPass());
                       

    mAdditiveBlendShader = mSurface->GetDevice()->createGraphicsPipeline(aInfo.mPipelineCache,
                                                                         aInfo.mFlags,
                                                                         { *aInfo.mVertexStage.get(), *aInfo.mFragmentStage.get() },
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
                                                                         mView->mRenderTarget->GetRenderPass());
  }


  void VkShader::Reload(VkCreatePipelineDataSet& aInfo)
  {
    Load(aInfo);
  }
}
