#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkRenderTarget.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineType(VkRenderTarget)
  {
    RegisterType<VkRenderTarget>();
    TypeBuilder<VkRenderTarget> builder;
  }


  VkRenderTarget::VkRenderTarget(VkRenderedSurface *aSurface,
                                 vk::Format aColorFormat,
                                 vk::Format aDepthFormat,
                                 std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                 ViewData* aView,
                                 std::string aName,
                                 DrawerTypeCombination aCombination)
    : mSurface(aSurface)
    , mColorFormat(aColorFormat)
    , mDepthFormat(aDepthFormat)
    , mVulkanSurface(aVulkanSurface)
    , mParentViewData(aView)
  {
    mData.mName = aName;
    mData.mCombinationType = aCombination;
    mData.mOrder = aView->mView->GetOrder();
    mCBOB = std::make_unique<VkCBOB<3, true>>(mSurface->GetRenderer()->mGraphicsQueueData->mCommandPool);
  }



  VkRenderTarget::VkRenderTarget(VkRenderedSurface *aSurface,
                                 vk::Format aColorFormat,
                                 vk::Format aDepthFormat,
                                 std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                 std::string aName,
                                 DrawerTypeCombination aCombination)
    : mSurface(aSurface)
    , mColorFormat(aColorFormat)
    , mDepthFormat(aDepthFormat)
    , mVulkanSurface(aVulkanSurface)
  {
    mData.mName = aName;
    mData.mCombinationType = aCombination;
    mCBOB = std::make_unique<VkCBOB<3, true>>(mSurface->GetRenderer()->mGraphicsQueueData->mCommandPool);
  }


  void VkRenderTarget::Initialize()
  {
    OPTICK_EVENT();

    CreateRenderPass();
    CreateFrameBuffer();
  }



  VkRenderTarget::~VkRenderTarget()
  {
    OPTICK_EVENT();

    for (int i = 0; i < mData.mAttachments.size(); ++i)
    {
      mData.mAttachments[i].mImageView.reset();
      mData.mAttachments[i].mImage.reset();
    }

    mData.mAttachments.clear();
    mData.mColorAttachments.clear();

    mRenderPass.reset();
    mData.mSampler.reset();
    mData.mFrameBuffer.reset();
  }


  bool VkRenderTarget::operator<(VkRenderTarget& rhs)
  {
    if (mParentViewData->mViewOrder < rhs.mParentViewData->mViewOrder)
    {
      return true;
    }
    return false;
  }


  void VkRenderTarget::Resize(vk::Extent2D& aExtent)
  {
    OPTICK_EVENT();

    mData.mExtent = aExtent;
    CreateFrameBuffer();
  }



  void VkRenderTarget::RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    UnusedArguments(aMeshes);
  }

  void VkRenderTarget::CreateFrameBuffer()
  {
    OPTICK_EVENT();

    for (int i = 0; i < mData.mAttachments.size(); ++i)
    {
      mData.mAttachments[i].mImageView.reset();
      mData.mAttachments[i].mImage.reset();
    }

    mData.mAttachments.clear();
    mData.mColorAttachments.clear();

    auto device = mSurface->GetDevice();

    if (0 == mData.mExtent.width ||
        0 == mData.mExtent.height)
    {
      mData.mExtent = mSurface->GetExtent();
    }

    ///////////////////
    // Color Image

    // create attachment
    vk::FormatProperties imageFormatProperties =
      mSurface->GetRenderer()->GetVkInternals()->GetPhysicalDevice()->getFormatProperties(mColorFormat);

    DebugObjection(false == ((imageFormatProperties.linearTilingFeatures &
                             vk::FormatFeatureFlagBits::eSampledImage) ||
                             (imageFormatProperties.optimalTilingFeatures &
                             vk::FormatFeatureFlagBits::eSampledImage)),
                   "Texture Format doesnt support system");

    vk::Extent3D imageExtent{ mData.mExtent.width, mData.mExtent.height, 1 };

    // create image
   auto colorImage = device->createImage({},
                                         vk::ImageType::e2D,
                                         mColorFormat,
                                         imageExtent,
                                         1,
                                         1,
                                         vk::SampleCountFlagBits::e1,
                                         vk::ImageTiling::eOptimal,
                                         vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                                         vk::SharingMode::eExclusive,
                                         {},
                                         vk::ImageLayout::eUndefined,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         nullptr);

    // create view
    vk::ComponentMapping components = { vk::ComponentSwizzle::eR, 
                                        vk::ComponentSwizzle::eG,
                                        vk::ComponentSwizzle::eB,
                                        vk::ComponentSwizzle::eA };
    u32 layers = 1;
    vk::ImageSubresourceRange subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, layers };


    auto colorView = colorImage->createImageView(vk::ImageViewType::e2D,
                                                 mColorFormat,
                                                 components,
                                                 subresourceRange);

    size_t att = mData.mAttachments.size();
    mData.mAttachments.emplace_back(colorImage, colorView);
    mData.mColorAttachments.emplace_back(att);

    // Create sampler for the color
    mData.mSampler = device->createSampler(vk::Filter::eLinear,
                                           vk::Filter::eLinear,
                                           vk::SamplerMipmapMode::eLinear,
                                           vk::SamplerAddressMode::eClampToEdge,
                                           vk::SamplerAddressMode::eClampToEdge,
                                           vk::SamplerAddressMode::eClampToEdge,
                                           0.0f,
                                           false,
                                           1.0f,
                                           false,
                                           vk::CompareOp::eNever,
                                           0.0f,
                                           1.0f,
                                           vk::BorderColor::eFloatOpaqueWhite,
                                           false);


    ///////////////////
    // Depth Image

    // create image
    auto depthImage = device->createImage({},
                                          vk::ImageType::e2D,
                                          mDepthFormat,
                                          imageExtent,
                                          1,
                                          1,
                                          vk::SampleCountFlagBits::e1,
                                          vk::ImageTiling::eOptimal,
                                          vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                          vk::SharingMode::eExclusive,
                                          {},
                                          vk::ImageLayout::eUndefined,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal,
                                          nullptr);

    // create view
    vk::ComponentMapping defaultMap;
    subresourceRange = { vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, layers };


    auto depthView = depthImage->createImageView(vk::ImageViewType::e2D,
                                                 mDepthFormat,
                                                 defaultMap,
                                                 subresourceRange);

    mData.mAttachments.emplace_back(depthImage, depthView);

    ///////////////////
    // FrameBuffer
    mData.mFrameBuffer = device->createFramebuffer(mRenderPass,
                                                   { mData.mAttachments[0].mImageView, mData.mAttachments[1].mImageView },
                                                   mData.mExtent,
                                                   1);
  }



  void VkRenderTarget::CreateRenderPass()
  {
    OPTICK_EVENT();

    // Attachment Descriptions
    vk::AttachmentDescription colorAttachment{ {},
                                               mColorFormat,
                                               vk::SampleCountFlagBits::e1,
                                               //vk::AttachmentLoadOp::eLoad,
                                               vk::AttachmentLoadOp::eClear,
                                               vk::AttachmentStoreOp::eStore, // color
                                               vk::AttachmentLoadOp::eDontCare,
                                               vk::AttachmentStoreOp::eDontCare, // stencil
                                               vk::ImageLayout::eUndefined,
                                               vk::ImageLayout::eShaderReadOnlyOptimal };

    vk::AttachmentDescription depthAttachment{ {},
                                               mDepthFormat,
                                               vk::SampleCountFlagBits::e1,
                                               vk::AttachmentLoadOp::eClear,
                                               vk::AttachmentStoreOp::eStore, // depth
                                               vk::AttachmentLoadOp::eDontCare,
                                               vk::AttachmentStoreOp::eDontCare, // stencil
                                               vk::ImageLayout::eUndefined,
                                               vk::ImageLayout::eDepthStencilAttachmentOptimal };

    std::array<vk::AttachmentDescription, 2> attachmentDescriptions{ colorAttachment,
                                                                     depthAttachment };

    // Subpass Description
    vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass{ {},
                                    vk::PipelineBindPoint::eGraphics,
                                    0,
                                    nullptr,
                                    1,
                                    &colorReference,
                                    nullptr,
                                    &depthReference,
                                    0,
                                    nullptr };

    std::array<vk::SubpassDependency, 2> subpassDependencies;

    // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commands executed outside of the actual renderpass)
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                           vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    // Transition from initial to final
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                           vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    mRenderPass = mSurface->GetDevice()->createRenderPass(attachmentDescriptions, subpass, subpassDependencies);
  }

  vk::ArrayProxy<const std::shared_ptr<vkhlf::CommandBuffer>> VkRenderTarget::GetCommands()
  {
    return (**mCBOB).first;
  }
}
