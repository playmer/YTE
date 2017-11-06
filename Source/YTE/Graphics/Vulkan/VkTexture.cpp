///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include <array>
#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"

#include "YTE/Utilities/Utilities.h"

namespace fs = std::experimental::filesystem;

namespace YTE
{
  YTEDefineType(VkTexture)
  {
    YTERegisterType(VkTexture);
  }



  VkTexture::VkTexture(std::string &aFile, VkRenderedSurface *aSurface)
    : Texture(aFile)
    , mSurface(aSurface)
  {
    auto device = mSurface->GetDevice();

    fs::path file{ mTexturePath };

    std::string textureName{ file.stem().string() };

    //TODO (Josh): Make Crunch work.
    //file = L"Crunch" / file.filename().concat(L".crn");
    file = L"Originals" / file.filename();
    std::string fileStr{ file.string() };

    auto allocator = mSurface->GetAllocator(AllocatorTypes::Texture);

    // 1. init image
    vk::Format format;

    switch (mType)
    {
      case TextureType::DXT1_sRGB:
      {
        format = vk::Format::eBc1RgbaSrgbBlock;
        break;
      }
      case TextureType::DXT5_sRGB:
      {
        format = vk::Format::eBc3SrgbBlock;
        break;
      }
      case TextureType::RGBA:
      {
        format = vk::Format::eR8G8B8A8Unorm;
        break;
      }
      default:
      {
        format = vk::Format::eR8G8B8A8Unorm;
      }
    }

    vk::FormatProperties imageFormatProperties =
      mSurface->GetRenderer()->GetVkInternals()->GetPhysicalDevice()->getFormatProperties(format);

    DebugObjection(false == ((imageFormatProperties.linearTilingFeatures &
                             vk::FormatFeatureFlagBits::eSampledImage) ||
                             (imageFormatProperties.optimalTilingFeatures &
                             vk::FormatFeatureFlagBits::eSampledImage)),
                   "Texture Format doesnt support system");

    vk::Extent3D imageExtent{ mWidth, mHeight, 1 };

    mImage = device->createImage({},
                                 vk::ImageType::e2D,
                                 format,
                                 imageExtent,
                                 1,
                                 1,
                                 vk::SampleCountFlagBits::e1,
                                 vk::ImageTiling::eOptimal,
                                 vk::ImageUsageFlagBits::eTransferDst |
                                 vk::ImageUsageFlagBits::eSampled,
                                 vk::SharingMode::eExclusive,
                                 {},
                                 vk::ImageLayout::eUndefined,
                                 vk::MemoryPropertyFlagBits::eDeviceLocal,
                                 allocator);

    mSurface->YTERegister(Events::GraphicsDataUpdateVk, this, &VkTexture::LoadToVulkan);

    mImageView = mImage->createImageView(vk::ImageViewType::e2D, format);

    // 2. init sampler
    mSampler = device->createSampler(vk::Filter::eNearest,
                                     vk::Filter::eNearest,
                                     vk::SamplerMipmapMode::eNearest,
                                     vk::SamplerAddressMode::eClampToEdge,
                                     vk::SamplerAddressMode::eClampToEdge,
                                     vk::SamplerAddressMode::eClampToEdge,
                                     0.0f,
                                     false,
                                     1.0f,
                                     false,
                                     vk::CompareOp::eNever,
                                     0.0f,
                                     0.0f,
                                     vk::BorderColor::eFloatOpaqueWhite,
                                     false);
  }



  VkTexture::~VkTexture()
  {
  }



  void VkTexture::LoadToVulkan(GraphicsDataUpdateVk *aEvent)
  {
    auto update = aEvent->mCBO;
    // create a temporary upload image and fill it with pixel data. 
    // The destructor of MappedImage will put the transfer into the command buffer.
    vkhlf::MappedImage mi(mImage, update, 0, mData.size());
    vk::SubresourceLayout layout = mi.getSubresourceLayout(vk::ImageAspectFlagBits::eColor, 0, 0);
    uint8_t * data = reinterpret_cast<uint8_t*>(mi.getPointer());

    auto height = mHeight;
    auto width = mWidth;

    auto pixels = mData.data();

    for (size_t y = 0; y < height; y++)
    {
      uint8_t * rowPtr = data;
      for (size_t x = 0; x < width; x++, rowPtr += 4, pixels += 4)
      {
        rowPtr[0] = pixels[0];
        rowPtr[1] = pixels[1];
        rowPtr[2] = pixels[2];
        rowPtr[3] = pixels[3];
      }
      data += layout.rowPitch;
    }

    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk, this, &VkTexture::LoadToVulkan);
  }
}
