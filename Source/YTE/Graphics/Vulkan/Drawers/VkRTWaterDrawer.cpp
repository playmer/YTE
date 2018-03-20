///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan - Drawers
///////////////////

#include "YTE/Graphics/Vulkan/Drawers/VkRTWaterDrawer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkLightManager.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Core/Engine.hpp"

static float EXTENT_FACTOR = 1.0f;

namespace YTE
{
  YTEDefineType(VkRTWaterDrawer)
  {
    YTERegisterType(VkRTWaterDrawer);
  }

  VkRTWaterDrawer::VkRTWaterDrawer(VkRenderedSurface *aSurface,
                                   vk::Format aColorFormat,
                                   vk::Format aDepthFormat,
                                   std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                   ViewData* aView,
                                   std::string aName,
                                   YTEDrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface,
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     aView,
                     "VkRTGameForwardDrawer_" + aName,
                     aCombinationType)
  {
    Initialize();
  }

  VkRTWaterDrawer::VkRTWaterDrawer(VkRenderedSurface *aSurface,
                                   vk::Format aColorFormat,
                                   vk::Format aDepthFormat,
                                   std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                   std::string aName,
                                   YTEDrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface,
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     "VkRTGameForwardDrawer_" + aName,
                     aCombinationType)
  {
    Initialize();
  }

  VkRTWaterDrawer::~VkRTWaterDrawer()
  {
    for (int i = 0; i < mReflectiveData.mAttachments.size(); ++i)
    {
      mReflectiveData.mAttachments[i].mImageView.reset();
      mReflectiveData.mAttachments[i].mImage.reset();
    }
    mReflectiveData.mAttachments.clear();
    mReflectiveData.mColorAttachments.clear();

    mReflectiveRenderPass.reset();
    mReflectiveData.mSampler.reset();
    mReflectiveData.mFrameBuffer.reset();
  }

  void VkRTWaterDrawer::RenderFull(const vk::Extent2D& aExtent,
                                   std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    if (mWaterComponent == nullptr)
    {
      return;
    }

    extent = aExtent;
    extent.width = static_cast<uint32_t>(static_cast<float>(extent.width) * EXTENT_FACTOR);
    extent.height = static_cast<uint32_t>(static_cast<float>(extent.height) * EXTENT_FACTOR);

    mCBOB->NextCommandBuffer();
    auto cbo = mCBOB->GetCurrentCBO();
    cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mRenderPass);
    RenderRefractive(cbo, aExtent, aMeshes);
    cbo->end();

    mReflectiveCBOB->NextCommandBuffer();
    auto reflectiveCBO = mReflectiveCBOB->GetCurrentCBO();
    reflectiveCBO->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mReflectiveRenderPass);
    RenderReflective(reflectiveCBO, aExtent, aMeshes);
    reflectiveCBO->end();
  }


  void VkRTWaterDrawer::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO, const vk::Extent2D& aExtent, std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    YTEUnusedArgument(aExtent);
    auto width = static_cast<float>(extent.width);
    auto height = static_cast<float>(extent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
    aCBO->setViewport(0, viewport);

    vk::Rect2D scissor{ { 0, 0 }, extent };
    aCBO->setScissor(0, scissor);
    aCBO->setLineWidth(1.0f);

    auto &instantiatedModels = mParentViewData->mInstantiatedModels;

    for (auto &shader : mParentViewData->mShaders)
    {
      for (auto &mesh : aMeshes)
      {
        auto &models = instantiatedModels[mesh.second.get()];

        // We can early out on this mesh if there are no models that use it.
        if (models.empty())
        {
          continue;
        }

        // We get the sub meshes that use the current shader, then draw them.
        auto range = mesh.second->mSubmeshes.equal_range(shader.first);

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          for (auto &model : models)
          {
            auto &data = model->mPipelineData[submesh.get()];

            // Gather up all the data for the individual passes.
            std::shared_ptr<vkhlf::Pipeline> *toUseToDraw{ nullptr };
            std::vector<DrawDataW> *toEmplaceInto{ nullptr };

            switch (model->mType)
            {
              case ShaderType::Triangles:
              {
                toUseToDraw = &shader.second->mTriangles;
                toEmplaceInto = &mTriangles;
                break;
              }
              case ShaderType::Lines:
              {
                toUseToDraw = &shader.second->mLines;
                toEmplaceInto = &mLines;
                break;
              }
              case ShaderType::Curves:
              {
                toUseToDraw = &shader.second->mCurves;
                toEmplaceInto = &mCurves;
                break;
              }
              case ShaderType::Wireframe:
              {
                toUseToDraw = &shader.second->mCurves;
                toEmplaceInto = &mCurves;
                break;
              }
              case ShaderType::ShaderNoCull:
              {
                toUseToDraw = &shader.second->mShaderNoCull;
                toEmplaceInto = &mShaderNoCull;
                break;
              }
              case ShaderType::AdditiveBlendShader:
              {
                toUseToDraw = &shader.second->mAdditiveBlendShader;
                toEmplaceInto = &mAdditiveBlendShader;
                break;
              }
              default:
              {
                assert(false);
              }
            }

            toEmplaceInto->emplace_back(*toUseToDraw,
                                        submesh->mVertexBuffer,
                                        submesh->mIndexBuffer,
                                        data.mPipelineLayout,
                                        data.mDescriptorSet,
                                        static_cast<u32>(submesh->mIndexCount),
                                        model->mLineWidth);
          }
        }
      }
    }


    auto runPipelines = [](std::shared_ptr<vkhlf::CommandBuffer> &aCBO,
                           std::vector<DrawDataW> &aShaders)
    {
      std::shared_ptr<vkhlf::Pipeline> *lastPipeline{ nullptr };
      float lastLineWidth = 1.0f;

      for (auto &drawCall : aShaders)
      {
        if (lastPipeline != &drawCall.mPipeline)
        {
          aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics,
                             drawCall.mPipeline);

          lastPipeline = &drawCall.mPipeline;
        }

        if (lastLineWidth != drawCall.mLineWidth)
        {
          aCBO->setLineWidth(drawCall.mLineWidth);
          lastLineWidth = drawCall.mLineWidth;
        }

        aCBO->bindVertexBuffer(0,
                               drawCall.mVertexBuffer,
                               0);

        aCBO->bindIndexBuffer(drawCall.mIndexBuffer,
                              0,
                              vk::IndexType::eUint32);

        aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                 drawCall.mPipelineLayout,
                                 0,
                                 drawCall.mDescriptorSet,
                                 nullptr);
        aCBO->drawIndexed(static_cast<u32>(drawCall.mIndexCount),
                          1,
                          0,
                          0,
                          0);
      }

      aShaders.clear();
    };

    runPipelines(aCBO, mTriangles);
    runPipelines(aCBO, mLines);
    runPipelines(aCBO, mCurves);
    runPipelines(aCBO, mShaderNoCull);
    runPipelines(aCBO, mAdditiveBlendShader);
  }

  void VkRTWaterDrawer::RenderRefractive(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                         const vk::Extent2D& aExtent,
                                         std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    YTEUnusedArgument(aExtent);

    // render
    Render(aCBO, extent, aMeshes);
  }

  void VkRTWaterDrawer::RenderReflective(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                         const vk::Extent2D& aExtent,
                                         std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    YTEUnusedArgument(aExtent);

    // render
    Render(aCBO, extent, aMeshes);
  }

  void VkRTWaterDrawer::Initialize()
  {
    extent = mSurface->GetExtent();

    mWaterComponent = nullptr;
    mData.mName = mName + "Refractive";
    mData.mCombinationType = mCombination;
    mCBOB = std::make_unique<VkCBOB<3, true>>(mSurface->GetCommandPool());
    mCBEB = std::make_unique<VkCBEB<3>>(mSurface->GetDevice());

    mReflectiveData.mName = mName + "Reflective";
    mReflectiveData.mCombinationType = mCombination;
    mReflectiveCBOB = std::make_unique<VkCBOB<3, true>>(mSurface->GetCommandPool());
    mReflectiveCBEB = std::make_unique<VkCBEB<3>>(mSurface->GetDevice());

    DeNotifyWaterComponent();
    CreateRefractiveRenderPass();
    CreateRefractiveFrameBuffer();
    CreateReflectiveRenderPass();
    CreateReflectiveFrameBuffer();

    NotifyWaterComponent();
  }

  void VkRTWaterDrawer::Resize(vk::Extent2D& aExtent)
  {
    extent = aExtent;
    extent.width = static_cast<uint32_t>(static_cast<float>(extent.width) * EXTENT_FACTOR);
    extent.height = static_cast<uint32_t>(static_cast<float>(extent.height) * EXTENT_FACTOR);

    DeNotifyWaterComponent();
    CreateRefractiveFrameBuffer();
    CreateReflectiveFrameBuffer();
    NotifyWaterComponent();
  }

  void VkRTWaterDrawer::ExecuteCommands(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    if (mWaterComponent == nullptr)
    {
      return;
    }


    // -----------------------------------
    // refractive
    // dont move camera

    // set clip plane
    UBOClipPlanes cp;
    cp.mNumOfPlanes = 1;
    cp.mPlanes[0] = glm::vec4(0, -1, 0, mWaterComponent->mTransform->GetTranslation().y);
    mParentViewData->mClipPlanesUBO->update<UBOClipPlanes>(0, cp, aCBO);

    // render
    auto& e = mCBEB->GetCurrentEvent();
    aCBO->setEvent(e, vk::PipelineStageFlagBits::eBottomOfPipe);

    vk::ClearDepthStencilValue depthStencil{ 1.0f, 0 };
    std::array<float, 4> colorValues;
    colorValues[0] = 0.0f;
    colorValues[1] = 0.0f;
    colorValues[2] = 0.0f;
    colorValues[3] = 0.0f;
    vk::ClearValue color{ colorValues };

    aCBO->beginRenderPass(mRenderPass,
                          mData.mFrameBuffer,
                          vk::Rect2D({ 0, 0 }, extent),
                          { color, depthStencil },
                          vk::SubpassContents::eSecondaryCommandBuffers);

    aCBO->executeCommands(mCBOB->GetCurrentCBO());

    aCBO->endRenderPass();













    // -----------------------------------
    // reflective
    // move camera
    UBOView view = mParentViewData->mLightManager.mGraphicsView->GetLastCamera()->InvertAcrossPlane(mWaterComponent->mTransform->GetTranslation().y);
    mParentViewData->mViewUBO->update<UBOView>(0, view, aCBO);


    // set clip plane
    cp.mNumOfPlanes = 1;
    cp.mPlanes[0] = glm::vec4(0, 1, 0, -mWaterComponent->mTransform->GetTranslation().y);
    mParentViewData->mClipPlanesUBO->update<UBOClipPlanes>(0, cp, aCBO);


    // render
    e = mReflectiveCBEB->GetCurrentEvent();
    aCBO->setEvent(e, vk::PipelineStageFlagBits::eBottomOfPipe);

    colorValues[0] = 0.0f;
    colorValues[1] = 0.0f;
    colorValues[2] = 0.0f;
    colorValues[3] = 0.0f;
    vk::ClearValue color2{ colorValues };

    aCBO->beginRenderPass(mReflectiveRenderPass,
                          mReflectiveData.mFrameBuffer,
                          vk::Rect2D({ 0, 0 }, extent),
                          { color2, depthStencil },
                          vk::SubpassContents::eSecondaryCommandBuffers);

    aCBO->executeCommands(mReflectiveCBOB->GetCurrentCBO());



    aCBO->endRenderPass();


    // unset clip plane
    cp.mNumOfPlanes = 0;
    mParentViewData->mClipPlanesUBO->update<UBOClipPlanes>(0, cp, aCBO);

    // move camera back
    view = mParentViewData->mLightManager.mGraphicsView->GetLastCamera()->ResetInversion();
    mParentViewData->mViewUBO->update<UBOView>(0, view, aCBO);

    // set the move amount for final render
    UBOIllumination ill = mParentViewData->mIlluminationUBOData;
    ill.mMoveAmount += static_cast<VkRenderer*>(mParentViewData->mLightManager.mGraphicsView->GetRenderer())->GetEngine()->GetDt();
    if (ill.mMoveAmount >= 1.0f)
    {
      ill.mMoveAmount = 0.0f;
    }
    mParentViewData->mIlluminationUBO->update<UBOIllumination>(0, ill, aCBO);
  }

  void VkRTWaterDrawer::ExecuteSecondaryEvent(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    YTEUnusedArgument(aCBO);
  }

  void VkRTWaterDrawer::MoveToNextEvent()
  {
    mCBEB->NextEvent();
    mReflectiveCBEB->NextEvent();
  }

  void VkRTWaterDrawer::NotifyWaterComponent()
  {
    if (mWaterComponent)
    {
      mWaterComponent->SetupSamplersFromVulkan(&mData.mSampler, 
                                               &mData.mAttachments[mData.mColorAttachments[0]].mImageView,
                                               &mReflectiveData.mSampler, 
                                               &mReflectiveData.mAttachments[mData.mColorAttachments[0]].mImageView);
    }
  }

  void VkRTWaterDrawer::DeNotifyWaterComponent()
  {
    if (mWaterComponent)
    {
      mWaterComponent->DeSetupSamplersFromVulkan();
    }
  }

  void VkRTWaterDrawer::CreateReflectiveRenderPass()
  {
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
      vk::ImageLayout::ePresentSrcKHR };

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

    mReflectiveRenderPass = mSurface->GetDevice()->createRenderPass(attachmentDescriptions, subpass, subpassDependencies);
  }

  void VkRTWaterDrawer::CreateReflectiveFrameBuffer()
  {
    for (int i = 0; i < mReflectiveData.mAttachments.size(); ++i)
    {
      mReflectiveData.mAttachments[i].mImageView.reset();
      mReflectiveData.mAttachments[i].mImage.reset();
    }
    mReflectiveData.mAttachments.clear();
    mReflectiveData.mColorAttachments.clear();

    auto device = mSurface->GetDevice();

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

    vk::Extent3D imageExtent{ extent.width, extent.height, 1 };

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

    size_t att = mReflectiveData.mAttachments.size();
    mReflectiveData.mAttachments.emplace_back(colorImage, colorView);
    mReflectiveData.mColorAttachments.emplace_back(att);

    // Create sampler for the color
    mReflectiveData.mSampler = device->createSampler(vk::Filter::eLinear,
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

    mReflectiveData.mAttachments.emplace_back(depthImage, depthView);

    ///////////////////
    // FrameBuffer
    mReflectiveData.mFrameBuffer = device->createFramebuffer(mReflectiveRenderPass,
                                                   { mReflectiveData.mAttachments[0].mImageView, mReflectiveData.mAttachments[1].mImageView },
                                                   extent,
                                                   1);


    // save data to descriptor
    mReflectiveData.mDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    mReflectiveData.mDescriptor.imageView = static_cast<vk::ImageView>(*mReflectiveData.mAttachments[0].mImageView);
    mReflectiveData.mDescriptor.sampler = static_cast<vk::Sampler>(*mReflectiveData.mSampler);
  }

  void VkRTWaterDrawer::CreateRefractiveRenderPass()
  {
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
      vk::ImageLayout::ePresentSrcKHR };

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

  void VkRTWaterDrawer::CreateRefractiveFrameBuffer()
  {
    for (int i = 0; i < mData.mAttachments.size(); ++i)
    {
      mData.mAttachments[i].mImageView.reset();
      mData.mAttachments[i].mImage.reset();
    }
    mData.mAttachments.clear();
    mData.mColorAttachments.clear();

    auto device = mSurface->GetDevice();

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

    vk::Extent3D imageExtent{ extent.width, extent.height, 1 };

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
                                                   extent,
                                                   1);


    // save data to descriptor
    mData.mDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    mData.mDescriptor.imageView = static_cast<vk::ImageView>(*mData.mAttachments[0].mImageView);
    mData.mDescriptor.sampler = static_cast<vk::Sampler>(*mData.mSampler);
  }
}