///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

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
                     VkShaderDescriptions &aDescriptions)
    : Shader(aName)
    , mSurface(aSurface)
    , mPipelineLayout(aLayout)
    , mDescriptions(aDescriptions)
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

    auto vertexData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eVertex, vertexFile);
    auto fragmentData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eFragment, fragmentFile);

    if (false == vertexData.mValid)
    {
      std::cout << vertexData.mReason << "\n";
      return;
    }
    else if (false == fragmentData.mValid)
    {
      std::cout << fragmentData.mReason << "\n";
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
                                                           //vk::CullModeFlagBits::eNone,
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

    vk::PipelineDepthStencilStateCreateInfo depthStencil({},
                                                         true,
                                                         true,
                                                         vk::CompareOp::eLessOrEqual,
                                                         false,
                                                         false,
                                                         stencilOpState,
                                                         stencilOpState,
                                                         0.0f,
                                                         0.0f);

    vk::PipelineColorBlendAttachmentState colorBlendAttachment(false,
                                                               vk::BlendFactor::eDstAlpha,
                                                               vk::BlendFactor::eOne,
                                                               vk::BlendOp::eAdd,
                                                               vk::BlendFactor::eZero,
                                                               vk::BlendFactor::eOneMinusSrcAlpha,
                                                               vk::BlendOp::eAdd,
                                                               vk::ColorComponentFlagBits::eR |
                                                               vk::ColorComponentFlagBits::eG |
                                                               vk::ColorComponentFlagBits::eB |
                                                               vk::ColorComponentFlagBits::eA);

    vkhlf::PipelineColorBlendStateCreateInfo colorBlend(false,
                                                        vk::LogicOp::eNoOp,
                                                        colorBlendAttachment,
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
                                             depthStencil,
                                             colorBlend,
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
