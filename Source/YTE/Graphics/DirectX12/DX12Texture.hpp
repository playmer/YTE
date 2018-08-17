#pragma once

#ifndef YTE_Graphics_Vulkan_VkTexture_hpp
#define YTE_Graphics_Vulkan_VkTexture_hpp

#include "YTE/Graphics/Generics/Texture.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"

namespace YTE
{
  class DX12Texture : public EventHandler
  {
  public:
    YTEDeclareType(DX12Texture);

    DX12Texture(Texture *aTexture,
              Dx12Renderer *aRenderer,
              vk::ImageViewType aVulkanType);

    ~DX12Texture();

    void Initialize();

    void LoadToVulkan(DX12GraphicsDataUpdate *aEvent);

    std::shared_ptr<vkhlf::Sampler> mSampler;
    std::shared_ptr<vkhlf::ImageView> mImageView;
    std::shared_ptr<vkhlf::Image> mImage;
    vk::ImageLayout mImageLayout;
    vk::DeviceMemory mDeviceMemory;
    vk::DescriptorImageInfo mDescriptor;
    Dx12Renderer *mRenderer;
    vk::ImageViewType mVulkanType;

    Texture *mTexture;
  };
}

#endif
