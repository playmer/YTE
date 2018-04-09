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
  class VkTexture : public Texture
  {
  public:
    YTEDeclareType(VkTexture);

    VkTexture(std::vector<u8> aData,
              TextureLayout aType,
              u32 aWidth,
              u32 aHeight,
              u32 aMipLevels,
              u32 aLayerCount,
              VkRenderer *aRenderer,
              vk::ImageViewType aVulkanType);

    VkTexture(std::string &aFile,
              VkRenderer *aRenderer,
              vk::ImageViewType aType);
    ~VkTexture() override;

    void Initialize();

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    std::shared_ptr<vkhlf::Sampler> mSampler;
    std::shared_ptr<vkhlf::ImageView> mImageView;
    std::shared_ptr<vkhlf::Image> mImage;
    vk::ImageLayout mImageLayout;
    vk::DeviceMemory mDeviceMemory;
    vk::DescriptorImageInfo mDescriptor;
    VkRenderer *mRenderer;
    vk::ImageViewType mVulkanType;
  };
}

#endif
