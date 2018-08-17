#pragma once

#ifndef YTE_Graphics_Vulkan_Dx12Shader_hpp
#define YTE_Graphics_Vulkan_Dx12Shader_hpp

#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/Shader.hpp"

namespace YTE
{
  struct VkCreatePipelineDataSet
  {
    VkCreatePipelineDataSet(std::shared_ptr<vkhlf::PipelineCache> aPipelineCache,
                            vk::PipelineCreateFlags aFlags,
                            std::shared_ptr<vkhlf::PipelineShaderStageCreateInfo> aVertexStage,
                            std::shared_ptr<vkhlf::PipelineShaderStageCreateInfo> aFragmentStage,
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
                            std::shared_ptr<vkhlf::PipelineLayout> aPipelineLayout,
                            std::string aName,
                            std::string aErrorMessage,
                            Dx12RenderedSurface* aSurface,
                            Dx12ShaderDescriptions aDescriptions,
                            bool aValid)
      : mPipelineCache(aPipelineCache)
      , mFlags(aFlags)
      , mVertexStage(aVertexStage)
      , mFragmentStage(aFragmentStage)
      , mVertexInput(aVertexInput)
      , mAssembly(aAssembly)
      , mTessalationState(aTessalationState)
      , mViewport(aViewport)
      , mRasterizationNoCull(aRasterizationNoCull)
      , mRasterizationCullBack(aRasterizationCullBack)
      , mMultiSample(aMultiSample)
      , mEnableDepthStencil(aEnableDepthStencil)
      , mDisableDepthStencil(aDisableDepthStencil)
      , mNoColorBlend(aNoColorBlend)
      , mAlphaColorBlend(aAlphaColorBlend)
      , mAdditiveColorBlend(aAdditiveColorBlend)
      , mDynamicState(aDynamicState)
      , mPipelineLayout(aPipelineLayout)
      , mName(aName)
      , mErrorMessage(aErrorMessage)
      , mSurface(aSurface)
      , mDescriptions(aDescriptions)
      , mValid(aValid)
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
    std::shared_ptr<vkhlf::PipelineShaderStageCreateInfo> mVertexStage = nullptr;
    std::shared_ptr<vkhlf::PipelineShaderStageCreateInfo> mFragmentStage = nullptr;
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

    // data / reconstruction
    std::string mName = "";
    std::string mErrorMessage = "";
    Dx12RenderedSurface* mSurface = nullptr;
    Dx12ShaderDescriptions mDescriptions;
    bool mValid = false;
  };

  class Dx12Shader : public Shader
  {
  public:
    YTEDeclareType(Dx12Shader);

    Dx12Shader(VkCreatePipelineDataSet& aInfo, DX12ViewData* aView);
    ~Dx12Shader() override;

    static VkCreatePipelineDataSet CreateInfo(std::string &aName,
                                              Dx12RenderedSurface *aSurface,
                                              std::shared_ptr<vkhlf::PipelineLayout> aLayout,
                                              Dx12ShaderDescriptions &aDescriptions,
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
    Dx12RenderedSurface *mSurface;
    DX12ViewData* mView;
  };
}

#endif
