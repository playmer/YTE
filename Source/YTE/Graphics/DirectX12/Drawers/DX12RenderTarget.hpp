///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan - Drawers
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_Drawers_VkRenderTarget_hpp
#define YTE_Graphics_Vulkan_Drawers_VkRenderTarget_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12ShaderDescriptions.hpp"
#include "YTE/Graphics/DirectX12/DX12VkCommandBufferBuffer.hpp"
#include "YTE/Graphics/DirectX12/DX12VkCommandBufferEventBuffer.hpp"

namespace YTE
{
  class VkRenderTarget : public EventHandler
  {
  public:
    struct RenderTargetData
    {
      struct Attachment
      {
        Attachment() : mImageView(nullptr), mImage(nullptr) {}
        Attachment(std::shared_ptr<vkhlf::Image> aImage, std::shared_ptr<vkhlf::ImageView> aImageView)
          : mImageView(aImageView)
          , mImage(aImage)
        {

        }

        //std::shared_ptr<vkhlf::ImageView> mImageView;
        //std::shared_ptr<vkhlf::Image> mImage;
      };

      //std::shared_ptr<vkhlf::Framebuffer> mFrameBuffer;
      //std::shared_ptr<vkhlf::Sampler> mSampler;
      vk::Extent2D mExtent;
      std::vector<Attachment> mAttachments;
      std::vector<size_t> mColorAttachments;
      std::string mName;
      DrawerTypeCombination mCombinationType;
      float mOrder;

      bool operator<(RenderTargetData& rhs)
      {
        if (mOrder < rhs.mOrder)
        {
          return true;
        }
        return false;
      }
    };


    YTEDeclareType(VkRenderTarget);

    VkRenderTarget(Dx12RenderedSurface *aSurface,
                   vk::Format aColorFormat,
                   vk::Format aDepthFormat,
                   //std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                   ViewData *aView,
                   std::string aName,
                   DrawerTypeCombination aCombination);

    VkRenderTarget(Dx12RenderedSurface *aSurface,
                   vk::Format aColorFormat,
                   vk::Format aDepthFormat,
                   //std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                   std::string aName,
                   DrawerTypeCombination aCombination);

    virtual ~VkRenderTarget();

    // for sorting
    //bool operator() (int i, int j);
    bool operator<(VkRenderTarget& rhs);

    virtual void Initialize();

    virtual void Resize(vk::Extent2D& aExtent);
    virtual void RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);

    virtual void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    virtual void ExecuteCommands(/*std::shared_ptr<vkhlf::CommandBuffer>& aCBO*/);
    virtual void ExecuteSecondaryEvent(/*std::shared_ptr<vkhlf::CommandBuffer>& aCBO*/);
    virtual void MoveToNextEvent();

    std::shared_ptr<vkhlf::RenderPass>& GetRenderPass()
    {
      return mRenderPass;
    }

    RenderTargetData* GetRenderTargetData()
    {
      return &mData;
    }

    void SetView(ViewData *aView)
    {
      mParentViewData = aView;
    }

    void SetOrder(float aOrder)
    {
      mData.mOrder = aOrder;
    }

    void SetCombinationType(DrawerTypeCombination aCombination)
    {
      mData.mCombinationType = aCombination;
    }

    std::shared_ptr<vkhlf::Framebuffer>& GetFrameBuffer()
    {
      return mData.mFrameBuffer;
    }

  protected:
    virtual void CreateFrameBuffer();
    virtual void CreateRenderPass();

    Dx12RenderedSurface *mSurface;
    ViewData *mParentViewData;

    RenderTargetData mData;
    std::shared_ptr<vkhlf::Surface> mVulkanSurface;
    std::shared_ptr<vkhlf::RenderPass> mRenderPass;
    std::unique_ptr<Dx12CBOB<3, true>> mCBOB;
    std::unique_ptr<VkCBEB<3 >> mCBEB;
    vk::Format mColorFormat;
    vk::Format mDepthFormat;
    bool mSignedUpForUpdate;
  };
}


#endif