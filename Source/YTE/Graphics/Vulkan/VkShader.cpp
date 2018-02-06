///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////
#include "fmt/format.h"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Graphics/Vulkan/VkShaderCompiler.hpp"
#include "YTE/Graphics/Vulkan/VkShaderDescriptions.hpp"

namespace YTE
{
  YTEDefineType(VkShader)
  {
    YTERegisterType(VkShader);
  }

  VkShader::VkShader(std::string &aName,
                     VkRenderedSurface *aSurface,
                     std::shared_ptr<vkhlf::PipelineLayout> aLayout,
                     VkShaderDescriptions &aDescriptions,
                     bool aCullBackfaces)
    : Shader(aName)
    , mSurface(aSurface)
    , mPipelineLayout(aLayout)
    , mDescriptions(aDescriptions)
    , mCullBackFaces(aCullBackfaces)
  {
    mSurface->YTERegister(Events::GraphicsDataUpdateVk, this, &VkShader::LoadToVulkan);
  }

  VkShader::~VkShader()
  {
  }


  void VkShader::Load()
  {
    auto device = mSurface->GetDevice();

    auto vertex = mShaderSetName + ".vert";
    auto fragment = mShaderSetName + ".frag";

    auto vertexFile = Path::GetShaderPath(Path::GetEnginePath(), vertex.c_str());
    auto fragmentFile = Path::GetShaderPath(Path::GetEnginePath(), fragment.c_str());


    auto lines = mDescriptions.GetLines();

    auto vertexData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eVertex, vertexFile, lines);
    auto fragmentData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eFragment, fragmentFile, lines);

    if (false == vertexData.mValid || false == fragmentData.mValid)
    {
      auto str = fmt::format("Vertex Shader {}:\n {}\nFragment Shader {}:\n {}\n", 
                             vertexFile, 
                             vertexData.mReason,
                             fragmentFile, 
                             fragmentData.mReason);

      std::cout << str;
      return;
    }

    auto vertexModule = device->createShaderModule(vertexData.mData);
    auto fragmentModule = device->createShaderModule(fragmentData.mData);


    // Initialize Pipeline
    std::shared_ptr<vkhlf::PipelineCache> pipelineCache = device->createPipelineCache(0, nullptr);

    vkhlf::PipelineShaderStageCreateInfo vertexStage(vk::ShaderStageFlagBits::eVertex,
                                                     vertexModule,
                                                     "main");
    vkhlf::PipelineShaderStageCreateInfo fragmentStage(vk::ShaderStageFlagBits::eFragment,
                                                       fragmentModule,
                                                       "main");

    vkhlf::PipelineVertexInputStateCreateInfo vertexInput(mDescriptions.Bindings(),
                                                          mDescriptions.Attributes());
    vk::PipelineInputAssemblyStateCreateInfo assembly({},
                                                      vk::PrimitiveTopology::eTriangleList,
                                                      VK_FALSE);

    // One dummy viewport and scissor, as dynamic state sets them.
    vkhlf::PipelineViewportStateCreateInfo viewport({ {} }, { {} });
    vk::PipelineRasterizationStateCreateInfo rasterization({},
                                                           false,
                                                           false,
                                                           vk::PolygonMode::eFill,
                                                           mCullBackFaces ? 
                                                             vk::CullModeFlagBits::eBack : 
                                                             vk::CullModeFlagBits::eNone,
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

    mShader = device->createGraphicsPipeline(pipelineCache,
                                             {},
                                             { vertexStage, fragmentStage },
                                             vertexInput,
                                             assembly,
                                             nullptr,
                                             viewport,
                                             rasterization,
                                             multisample,
                                             enableDepthStencil,
                                             noColorBlend,
                                             dynamic,
                                             mPipelineLayout,
                                             mSurface->GetRenderPass());
    mAdditiveBlendShader = device->createGraphicsPipeline(pipelineCache,
                                                          {},
                                                          { vertexStage, fragmentStage },
                                                          vertexInput,
                                                          assembly,
                                                          nullptr,
                                                          viewport,
                                                          rasterization,
                                                          multisample,
                                                          disableDepthStencil,
                                                          additiveColorBlend,
                                                          dynamic,
                                                          mPipelineLayout,
                                                          mSurface->GetRenderPass());
    mAlphaBlendShader = device->createGraphicsPipeline(pipelineCache,
                                                       {},
                                                       { vertexStage, fragmentStage },
                                                       vertexInput,
                                                       assembly,
                                                       nullptr,
                                                       viewport,
                                                       rasterization,
                                                       multisample,
                                                       disableDepthStencil,
                                                       alphaColorBlend,
                                                       dynamic,
                                                       mPipelineLayout,
                                                       mSurface->GetRenderPass());
  }

  void VkShader::Reload()
  {
    Load();
  }

  void VkShader::LoadToVulkan(GraphicsDataUpdateVk *aEvent)
  {
    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk, this, &VkShader::LoadToVulkan);
    YTEUnusedArgument(aEvent);

    Load();
  }
}
