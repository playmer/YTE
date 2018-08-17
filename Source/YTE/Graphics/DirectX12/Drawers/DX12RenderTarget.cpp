#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/DirectX12/Drawers/DX12RenderTarget.hpp"
#include "YTE/Graphics/DirectX12/DX12Internals.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineType(DX12RenderTarget)
  {
    RegisterType<DX12RenderTarget>();
    TypeBuilder<DX12RenderTarget> builder;
  }


  DX12RenderTarget::DX12RenderTarget(Dx12RenderedSurface *aSurface,
                                     //vk::Format aColorFormat,
                                     //vk::Format aDepthFormat,
                                     //std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                     DX12ViewData* aView,
                                     std::string aName,
                                     DrawerTypeCombination aCombination)
    : mSurface(aSurface)
    , mColorFormat(aColorFormat)
    , mDepthFormat(aDepthFormat)
    , mVulkanSurface(aVulkanSurface)
    , mParentViewData(aView)
  {
    //mSignedUpForUpdate = true;
    //mData.mName = aName;
    //mData.mCombinationType = aCombination;
    //mData.mOrder = aView->mView->GetOrder();
    //mCBOB = std::make_unique<Dx12CBOB<3, true>>(mSurface->GetCommandPool());
    //mCBEB = std::make_unique<VkCBEB<3>>(mSurface->GetDevice());
  }



  DX12RenderTarget::DX12RenderTarget(Dx12RenderedSurface *aSurface,
                                     //vk::Format aColorFormat,
                                     //vk::Format aDepthFormat,
                                     //std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                     std::string aName,
                                     DrawerTypeCombination aCombination)
    : mSurface(aSurface)
    , mColorFormat(aColorFormat)
    , mDepthFormat(aDepthFormat)
    , mVulkanSurface(aVulkanSurface)
  {
    //mSignedUpForUpdate = true;
    //mData.mName = aName;
    //mData.mCombinationType = aCombination;
    //mCBOB = std::make_unique<Dx12CBOB<3, true>>(mSurface->GetCommandPool());
    //mCBEB = std::make_unique<VkCBEB<3>>(mSurface->GetDevice());
  }


  void DX12RenderTarget::Initialize()
  {
    CreateRenderPass();
    CreateFrameBuffer();
  }



  DX12RenderTarget::~DX12RenderTarget()
  {
    //for (int i = 0; i < mData.mAttachments.size(); ++i)
    //{
    //  mData.mAttachments[i].mImageView.reset();
    //  mData.mAttachments[i].mImage.reset();
    //}
    //mData.mAttachments.clear();
    //mData.mColorAttachments.clear();
    //
    //mRenderPass.reset();
    //mData.mSampler.reset();
    //mData.mFrameBuffer.reset();
  }


  bool DX12RenderTarget::operator<(DX12RenderTarget& rhs)
  {
    //if (mParentViewData->mViewOrder < rhs.mParentViewData->mViewOrder)
    //{
    //  return true;
    //}
    //return false;
  }


  void DX12RenderTarget::Resize(vk::Extent2D& aExtent)
  {
    //mData.mExtent = aExtent;
    //CreateFrameBuffer();
  }

  void DX12RenderTarget::RenderFull(std::unordered_map<std::string, std::unique_ptr<DX12Mesh>>& aMeshes)
  {
    UnusedArguments(aMeshes);
  }

  void DX12RenderTarget::MoveToNextEvent()
  {
    //mCBEB->NextEvent();
  }

  //void DX12RenderTarget::ExecuteCommands(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  //{
  //  aCBO->executeCommands(mCBOB->GetCurrentCBO());
  //}

  //void DX12RenderTarget::ExecuteSecondaryEvent(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  //{
  //  auto& e = mCBEB->GetCurrentEvent();
  //  aCBO->setEvent(e, vk::PipelineStageFlagBits::eBottomOfPipe);
  //}

  void DX12RenderTarget::LoadToVulkan(DX12GraphicsDataUpdate *aEvent)
  {
    mSurface->DeregisterEvent<&DX12RenderTarget::LoadToVulkan>(Events::DX12GraphicsDataUpdate,  this);
    mSignedUpForUpdate = false;
    UnusedArguments(aEvent);
  }

  void DX12RenderTarget::CreateFrameBuffer()
  {
    //for (int i = 0; i < mData.mAttachments.size(); ++i)
    //{
    //  mData.mAttachments[i].mImageView.reset();
    //  mData.mAttachments[i].mImage.reset();
    //}
    //mData.mAttachments.clear();
    //mData.mColorAttachments.clear();
    //
    //auto device = mSurface->GetDevice();
    //
    //if (0 == mData.mExtent.width ||
    //    0 == mData.mExtent.height)
    //{
    //  mData.mExtent = mSurface->GetExtent();
    //}
    //
    /////////////////////
    //// Color Image
    //
    //// create attachment
    //vk::FormatProperties imageFormatProperties =
    //  mSurface->GetRenderer()->GetDx12Internals()->GetPhysicalDevice()->getFormatProperties(mColorFormat);
    //
    //DebugObjection(false == ((imageFormatProperties.linearTilingFeatures &
    //                         vk::FormatFeatureFlagBits::eSampledImage) ||
    //                         (imageFormatProperties.optimalTilingFeatures &
    //                         vk::FormatFeatureFlagBits::eSampledImage)),
    //               "Texture Format doesnt support system");
    //
    //vk::Extent3D imageExtent{ mData.mExtent.width, mData.mExtent.height, 1 };
    //
    //// create image
    //auto colorImage = device->createImage({},
    //                                      vk::ImageType::e2D,
    //                                      mColorFormat,
    //                                      imageExtent,
    //                                      1,
    //                                      1,
    //                                      vk::SampleCountFlagBits::e1,
    //                                      vk::ImageTiling::eOptimal,
    //                                      vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
    //                                      vk::SharingMode::eExclusive,
    //                                      {},
    //                                      vk::ImageLayout::eUndefined,
    //                                      vk::MemoryPropertyFlagBits::eDeviceLocal,
    //                                      nullptr);
    //
    //// create view
    //vk::ComponentMapping components = { vk::ComponentSwizzle::eR, 
    //                                    vk::ComponentSwizzle::eG,
    //                                    vk::ComponentSwizzle::eB,
    //                                    vk::ComponentSwizzle::eA };
    //u32 layers = 1;
    //vk::ImageSubresourceRange subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, layers };
    //
    //
    //auto colorView = colorImage->createImageView(vk::ImageViewType::e2D,
    //                                             mColorFormat,
    //                                             components,
    //                                             subresourceRange);
    //
    //size_t att = mData.mAttachments.size();
    //mData.mAttachments.emplace_back(colorImage, colorView);
    //mData.mColorAttachments.emplace_back(att);
    //
    //// Create sampler for the color
    //mData.mSampler = device->createSampler(vk::Filter::eLinear,
    //                                       vk::Filter::eLinear,
    //                                       vk::SamplerMipmapMode::eLinear,
    //                                       vk::SamplerAddressMode::eClampToEdge,
    //                                       vk::SamplerAddressMode::eClampToEdge,
    //                                       vk::SamplerAddressMode::eClampToEdge,
    //                                       0.0f,
    //                                       false,
    //                                       1.0f,
    //                                       false,
    //                                       vk::CompareOp::eNever,
    //                                       0.0f,
    //                                       1.0f,
    //                                       vk::BorderColor::eFloatOpaqueWhite,
    //                                       false);
    //
    //
    /////////////////////
    //// Depth Image
    //
    //// create image
    //auto depthImage = device->createImage({},
    //                                      vk::ImageType::e2D,
    //                                      mDepthFormat,
    //                                      imageExtent,
    //                                      1,
    //                                      1,
    //                                      vk::SampleCountFlagBits::e1,
    //                                      vk::ImageTiling::eOptimal,
    //                                      vk::ImageUsageFlagBits::eDepthStencilAttachment,
    //                                      vk::SharingMode::eExclusive,
    //                                      {},
    //                                      vk::ImageLayout::eUndefined,
    //                                      vk::MemoryPropertyFlagBits::eDeviceLocal,
    //                                      nullptr);
    //
    //// create view
    //vk::ComponentMapping defaultMap;
    //subresourceRange = { vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, layers };
    //
    //
    //auto depthView = depthImage->createImageView(vk::ImageViewType::e2D,
    //                                             mDepthFormat,
    //                                             defaultMap,
    //                                             subresourceRange);
    //
    //mData.mAttachments.emplace_back(depthImage, depthView);
    //
    /////////////////////
    //// FrameBuffer
    //mData.mFrameBuffer = device->createFramebuffer(mRenderPass,
    //                                               { mData.mAttachments[0].mImageView, mData.mAttachments[1].mImageView },
    //                                               mData.mExtent,
    //                                               1);
  }



  void DX12RenderTarget::CreateRenderPass()
  {
    //// Attachment Descriptions
    //vk::AttachmentDescription colorAttachment{ {},
    //                                           mColorFormat,
    //                                           vk::SampleCountFlagBits::e1,
    //                                           //vk::AttachmentLoadOp::eLoad,
    //                                           vk::AttachmentLoadOp::eClear,
    //                                           vk::AttachmentStoreOp::eStore, // color
    //                                           vk::AttachmentLoadOp::eDontCare,
    //                                           vk::AttachmentStoreOp::eDontCare, // stencil
    //                                           vk::ImageLayout::eUndefined,
    //                                           vk::ImageLayout::ePresentSrcKHR };
    //
    //vk::AttachmentDescription depthAttachment{ {},
    //                                           mDepthFormat,
    //                                           vk::SampleCountFlagBits::e1,
    //                                           vk::AttachmentLoadOp::eClear,
    //                                           vk::AttachmentStoreOp::eStore, // depth
    //                                           vk::AttachmentLoadOp::eDontCare,
    //                                           vk::AttachmentStoreOp::eDontCare, // stencil
    //                                           vk::ImageLayout::eUndefined,
    //                                           vk::ImageLayout::eDepthStencilAttachmentOptimal };
    //
    //std::array<vk::AttachmentDescription, 2> attachmentDescriptions{ colorAttachment,
    //                                                                 depthAttachment };
    //
    //// Subpass Description
    //vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    //vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    //
    //vk::SubpassDescription subpass{ {},
    //                                vk::PipelineBindPoint::eGraphics,
    //                                0,
    //                                nullptr,
    //                                1,
    //                                &colorReference,
    //                                nullptr,
    //                                &depthReference,
    //                                0,
    //                                nullptr };
    //
    //std::array<vk::SubpassDependency, 2> subpassDependencies;
    //
    //// Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commands executed outside of the actual renderpass)
    //subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    //subpassDependencies[0].dstSubpass = 0;
    //subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    //subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    //subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    //subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
    //                                       vk::AccessFlagBits::eColorAttachmentWrite;
    //subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;
    //
    //// Transition from initial to final
    //subpassDependencies[1].srcSubpass = 0;
    //subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    //subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    //subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    //subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
    //                                       vk::AccessFlagBits::eColorAttachmentWrite;
    //subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    //subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;
    //
    //mRenderPass = mSurface->GetDevice()->createRenderPass(attachmentDescriptions, subpass, subpassDependencies);
  }
}
