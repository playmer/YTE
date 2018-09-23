///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkTexture_hpp
#define YTE_Graphics_Vulkan_VkTexture_hpp

#include "YTE/Graphics/Generics/Texture.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

namespace YTE
{
  class VkTexture : public EventHandler
  {
  public:
    YTEDeclareType(VkTexture);

    VkTexture(Texture *aTexture,
              VkRenderer *aRenderer,
              vk::ImageViewType aVulkanType);

    ~VkTexture();

    void Initialize();

    void LoadToVulkan(VkGraphicsDataUpdate *aEvent);

    std::shared_ptr<vkhlf::Sampler> mSampler;
    std::shared_ptr<vkhlf::ImageView> mImageView;
    std::shared_ptr<vkhlf::Image> mImage;
    vk::ImageLayout mImageLayout;
    vk::DeviceMemory mDeviceMemory;
    vk::DescriptorImageInfo mDescriptor;
    VkRenderer *mRenderer;
    vk::ImageViewType mVulkanType;

    Texture *mTexture;
  };
}

#endif
