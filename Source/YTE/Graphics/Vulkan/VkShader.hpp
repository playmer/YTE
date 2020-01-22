#pragma once

#ifndef YTE_Graphics_Vulkan_VkShader_hpp
#define YTE_Graphics_Vulkan_VkShader_hpp

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/Shader.hpp"

namespace YTE
{
  struct VkCreatePipelineDataSet
  {
    VkCreatePipelineDataSet(
      std::shared_ptr<vkhlf::PipelineCache> aPipelineCache,
      vk::PipelineCreateFlags aFlags,
      std::vector<vkhlf::PipelineShaderStageCreateInfo>& aStages,
      vkhlf::PipelineVertexInputStateCreateInfo aVertexInput,
      vk::PipelineInputAssemblyStateCreateInfo aAssembly,
      vk::Optional<const vk::PipelineTessellationStateCreateInfo> aTessalationState,
      vkhlf::PipelineViewportStateCreateInfo aViewport,
      vk::PipelineRasterizationStateCreateInfo aRasterizationNoCull,
      vk::PipelineRasterizationStateCreateInfo aRasterizationCullBack,
      vkhlf::PipelineMultisampleStateCreateInfo aMultiSample,
      vk::PipelineDepthStencilStateCreateInfo aEnableDepthStencil,
      vk::PipelineDepthStencilStateCreateInfo aDisableDepthStencil,
      vkhlf::PipelineColorBlendStateCreateInfo aNoColorBlend,
      vkhlf::PipelineColorBlendStateCreateInfo aAlphaColorBlend,
      vkhlf::PipelineColorBlendStateCreateInfo aAdditiveColorBlend,
      vkhlf::PipelineDynamicStateCreateInfo aDynamicState,
      std::shared_ptr<vkhlf::DescriptorSetLayout> aDescriptorSetLayout,
      std::shared_ptr<vkhlf::PipelineLayout> aPipelineLayout,
      std::string aName,
      std::string aErrorMessage,
      VkRenderedSurface* aSurface,
      VkShaderDescriptions& aDescriptions,
      bool aValid)      
      : mPipelineCache{ aPipelineCache }
      , mFlags{ aFlags }
      , mStages{ std::move(aStages) }
      , mVertexInput{ aVertexInput }
      , mAssembly{ aAssembly }
      , mTessalationState{ aTessalationState }
      , mViewport{ aViewport }
      , mRasterizationNoCull{ aRasterizationNoCull }
      , mRasterizationCullBack{ aRasterizationCullBack }
      , mMultiSample{ aMultiSample }
      , mEnableDepthStencil{ aEnableDepthStencil }
      , mDisableDepthStencil{ aDisableDepthStencil }
      , mNoColorBlend{ aNoColorBlend }
      , mAlphaColorBlend{ aAlphaColorBlend }
      , mAdditiveColorBlend{ aAdditiveColorBlend }
      , mDynamicState{ aDynamicState }
      , mDescriptorSetLayout{ aDescriptorSetLayout }
      , mPipelineLayout{ aPipelineLayout }
      , mName{ aName }
      , mErrorMessage{ aErrorMessage }
      , mSurface{ aSurface }
      , mDescriptions{ std::move(aDescriptions) }
      , mValid{ aValid }
    {

    }

    VkCreatePipelineDataSet()
    {

    }

    VkCreatePipelineDataSet(std::string aName, std::string aErrorMessage)
      : mName(aName)
      , mErrorMessage(aErrorMessage)
    {

    }

    // construction
    std::shared_ptr<vkhlf::PipelineCache> mPipelineCache = nullptr;
    vk::PipelineCreateFlags mFlags;
    std::vector<vkhlf::PipelineShaderStageCreateInfo> mStages;
    vkhlf::PipelineVertexInputStateCreateInfo mVertexInput = { {}, {} };
    vk::PipelineInputAssemblyStateCreateInfo mAssembly;
    vk::Optional<const vk::PipelineTessellationStateCreateInfo> mTessalationState = nullptr;
    vkhlf::PipelineViewportStateCreateInfo mViewport = { {}, {} };
    vk::PipelineRasterizationStateCreateInfo mRasterizationNoCull;
    vk::PipelineRasterizationStateCreateInfo mRasterizationCullBack;
    vkhlf::PipelineMultisampleStateCreateInfo mMultiSample = { vk::SampleCountFlagBits::e1, false, 0.0f, {}, false, false };
    vk::PipelineDepthStencilStateCreateInfo mEnableDepthStencil;
    vk::PipelineDepthStencilStateCreateInfo mDisableDepthStencil;
    vkhlf::PipelineColorBlendStateCreateInfo mNoColorBlend = { false, vk::LogicOp::eAnd, {}, {} };
    vkhlf::PipelineColorBlendStateCreateInfo mAlphaColorBlend = { false, vk::LogicOp::eAnd,{},{} };
    vkhlf::PipelineColorBlendStateCreateInfo mAdditiveColorBlend = { false, vk::LogicOp::eAnd, {}, {} };
    vkhlf::PipelineDynamicStateCreateInfo mDynamicState = { {} };
    std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout = nullptr;
    std::shared_ptr<vkhlf::DescriptorSetLayout> mDescriptorSetLayout = nullptr;

    // data / reconstruction
    std::string mName = "";
    std::string mErrorMessage = "";
    VkRenderedSurface* mSurface = nullptr;
    VkShaderDescriptions mDescriptions;
    bool mValid = false;
  };

  class VkShader : public Shader
  {
  public:
    YTEDeclareType(VkShader);

    VkShader(VkCreatePipelineDataSet& aInfo, ViewData* aView);
    ~VkShader() override;

    static VkCreatePipelineDataSet CreateInfo(
      std::string const&aName,
      VkRenderedSurface* aSurface,
      std::shared_ptr<vkhlf::DescriptorSetLayout> aDescriptorSetLayout,
      std::shared_ptr<vkhlf::PipelineLayout> aLayout,
      VkShaderDescriptions &aDescriptions,
      bool aReload);

    void Load(VkCreatePipelineDataSet& aInfo);

    void Reload(VkCreatePipelineDataSet& aInfo);
    
    std::shared_ptr<vkhlf::Pipeline> mTriangles;
    std::shared_ptr<vkhlf::Pipeline> mWireframe;
    std::shared_ptr<vkhlf::Pipeline> mLines;
    std::shared_ptr<vkhlf::Pipeline> mCurves;
    std::shared_ptr<vkhlf::Pipeline> mShaderNoCull;
    std::shared_ptr<vkhlf::Pipeline> mAlphaBlendShader;
    std::shared_ptr<vkhlf::Pipeline> mAdditiveBlendShader;

    VkRenderedSurface *mSurface;
    ViewData* mView;
  };
}

#endif
