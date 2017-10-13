#include <array>
#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/PrivateImplementation.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/ShaderDescriptions.hpp"
#include "YTE/Graphics/Texture.hpp"
#include "YTE/Graphics/View.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{

  void VkRenderer::AddDescriptorSet(RenderedSurface *aSurface,
    InstantiatedMesh *aModel,
    Mesh::SubMesh *aSubMesh,
    InstantiatedMeshRendererData::SubmeshPipelineData *aSubmeshPipelineData)
  {
    auto device = aSurface->mDevice;

    // init descriptor and pipeline layouts
    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    dslbs.emplace_back(0,
      vk::DescriptorType::eUniformBuffer,
      vk::ShaderStageFlagBits::eVertex,
      nullptr);
    dslbs.emplace_back(1,
      vk::DescriptorType::eUniformBuffer,
      vk::ShaderStageFlagBits::eVertex,
      nullptr);
    dslbs.emplace_back(2,
      vk::DescriptorType::eUniformBuffer,
      vk::ShaderStageFlagBits::eFragment,
      nullptr);

    u32 samplers = 0;

    if (nullptr != aSubMesh->mDiffuseMap) ++samplers;
    if (nullptr != aSubMesh->mSpecularMap) ++samplers;
    if (nullptr != aSubMesh->mNormalMap) ++samplers;

    std::shared_ptr<vkhlf::DescriptorPool> descriptorPool;

    if (0 != samplers)
    {
      std::vector<vk::DescriptorPoolSize> descriptorTypes;
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);

      for (u32 i = 0; i < samplers; ++i)
      {
        dslbs.emplace_back(i + 3,
          vk::DescriptorType::eCombinedImageSampler,
          vk::ShaderStageFlagBits::eFragment,
          nullptr);

        descriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 1);
      }

      descriptorPool = device->createDescriptorPool({}, 1, descriptorTypes);
    }
    else
    {
      descriptorPool = device->createDescriptorPool({},
        1,
        { { vk::DescriptorType::eUniformBuffer, 3 },
        });
    }

    auto modelData = aModel->mData.Get<InstantiatedMeshRendererData>();
    auto subMeshData = aSubMesh->mRendererData.Get<MeshRendererData>();

    auto descriptorSetLayout = device->createDescriptorSetLayout(dslbs);

    aSubmeshPipelineData->mPipelineLayout = device->createPipelineLayout(descriptorSetLayout, nullptr);

    // init descriptor set
    aSubmeshPipelineData->mDescriptorSet = device->allocateDescriptorSet(descriptorPool, descriptorSetLayout);
    std::vector<vkhlf::WriteDescriptorSet> wdss;
    wdss.reserve(6);

    // Helper constants and variables.
    constexpr auto unibuf = vk::DescriptorType::eUniformBuffer;
    auto &ds = aSubmeshPipelineData->mDescriptorSet;
    u32 binding = 0;

    // Add Uniform Buffers
    vkhlf::DescriptorBufferInfo uboView{ aSurface->mUBOView, 0, sizeof(UBOView) };
    vkhlf::DescriptorBufferInfo uboModel{ modelData->mUBOModel, 0, sizeof(UBOModel) };
    vkhlf::DescriptorBufferInfo uboMaterial{ subMeshData->mUBOMaterial, 0, sizeof(UBOMaterial) };

    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModel);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboMaterial);

    // Add Texture Samplers
    auto addTS = [&wdss, &binding, &ds](Texture *aData, vkhlf::DescriptorImageInfo &imageInfo)
    {
      constexpr auto imgsam = vk::DescriptorType::eCombinedImageSampler;

      if (nullptr == aData)
      {
        return;
      }

      auto t = aData->mRendererData.Get<TextureRendererData>();

      imageInfo.sampler = t->mSampler;
      imageInfo.imageView = t->mView;
      wdss.emplace_back(ds, binding++, 0, 1, imgsam, imageInfo, nullptr);
    };

    vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo sTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo nTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };

    addTS(aSubMesh->mDiffuseMap, dTexInfo);
    addTS(aSubMesh->mSpecularMap, sTexInfo);
    addTS(aSubMesh->mNormalMap, nTexInfo);

    device->updateDescriptorSets(wdss, nullptr);
  }

  std::shared_ptr<vkhlf::Pipeline> VkRenderer::AddPipeline(RenderedSurface *aSurface,
    InstantiatedMesh *aModel,
    Mesh::SubMesh *aSubMesh,
    InstantiatedMeshRendererData::SubmeshPipelineData *aSubmeshPipelineData)
  {
    auto device = aSurface->mDevice;

    auto vertex = aSubMesh->mShaderSetName + ".vert";
    auto fragment = aSubMesh->mShaderSetName + ".frag";

    auto vertexFile = Path::GetShaderPath(Path::GetEnginePath(), vertex.c_str());
    auto fragmentFile = Path::GetShaderPath(Path::GetEnginePath(), fragment.c_str());

    std::string vertexText;
    ReadFileToString(vertexFile, vertexText);

    std::string fragmetText;
    ReadFileToString(fragmentFile, fragmetText);

    auto vertexData = vkhlf::compileGLSLToSPIRV(vk::ShaderStageFlagBits::eVertex,
      vertexText);
    auto fragmentData = vkhlf::compileGLSLToSPIRV(vk::ShaderStageFlagBits::eFragment,
      fragmetText);

    auto vertexModule = device->createShaderModule(vertexData);
    auto fragmentModule = device->createShaderModule(fragmentData);


    // TODO (Josh): We should be reflecting these.
    ShaderDescriptions descriptions;
    descriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);

    //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mColor;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mTangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mBinormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mBitangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    // Initialize Pipeline
    std::shared_ptr<vkhlf::PipelineCache> pipelineCache = device->createPipelineCache(0, nullptr);

    vkhlf::PipelineShaderStageCreateInfo vertexStage(vk::ShaderStageFlagBits::eVertex,
      vertexModule,
      "main");
    vkhlf::PipelineShaderStageCreateInfo fragmentStage(vk::ShaderStageFlagBits::eFragment,
      fragmentModule,
      "main");

    vkhlf::PipelineVertexInputStateCreateInfo vertexInput(descriptions.Bindings(),
      descriptions.Attributes());
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

    auto pipeline = device->createGraphicsPipeline(pipelineCache,
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
      aSubmeshPipelineData->mPipelineLayout,
      aSurface->mRenderPass);

    return pipeline;
  }
}