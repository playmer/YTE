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

    VkTexture(std::string &aFile,
              VkRenderedSurface *aSurface,
              vk::ImageViewType aType);
    ~VkTexture() override;

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    std::shared_ptr<vkhlf::Sampler> mSampler;
    std::shared_ptr<vkhlf::ImageView> mImageView;
    std::shared_ptr<vkhlf::Image> mImage;
    vk::ImageLayout mImageLayout;
    vk::DeviceMemory mDeviceMemory;
    vk::DescriptorImageInfo mDescriptor;
    VkRenderedSurface *mSurface;
  };
}

#endif
