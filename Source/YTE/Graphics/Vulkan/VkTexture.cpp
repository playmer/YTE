#include <array>
#include "YTE/StandardLibrary/FileSystem.hpp"
#include <fstream>

#include "transcoder/basisu_transcoder.h"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace fs = std::filesystem;

namespace YTE
{
  YTEDefineType(VkTexture)
  {
    RegisterType<VkTexture>();
    TypeBuilder<VkTexture> builder;
  }


  VkTexture::VkTexture(Texture *aTexture,
                       VkRenderer *aRenderer,
                       vk::ImageViewType aVulkanType)
    : mRenderer{ aRenderer }
    , mVulkanType{ aVulkanType }
    , mTexture{ aTexture }
  {
    OPTICK_EVENT();

    Initialize();
  }

  void VkTexture::Initialize()
  {
    OPTICK_EVENT();

    auto device = mRenderer->mDevice;

    auto& allocator = GetAllocator(mRenderer->GetAllocator(AllocatorTypes::Texture));

    // 1. init image
    vk::Format format;

    switch (mTexture->mType)
    {
      case TextureLayout::Bc1_Rgba_Srgb:
      {
        format = vk::Format::eBc1RgbaSrgbBlock;
        break;
      }
      case TextureLayout::Bc3_Srgb:
      {
        format = vk::Format::eBc3SrgbBlock;
        break;
      }
      case TextureLayout::Bc3_Unorm:
      {
        format = vk::Format::eBc3UnormBlock;
        break;
      }
      case TextureLayout::Bc7_Unorm_Opaque:
      {
        format = vk::Format::eBc7UnormBlock;
        break;
      }
      case TextureLayout::RGBA:
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
      mRenderer->GetVkInternals()->GetPhysicalDevice()->getFormatProperties(format);

    DebugObjection(false == ((imageFormatProperties.linearTilingFeatures &
      vk::FormatFeatureFlagBits::eSampledImage) ||
      (imageFormatProperties.optimalTilingFeatures &
        vk::FormatFeatureFlagBits::eSampledImage)),
      "Texture Format doesnt support system");

    vk::Extent3D imageExtent{ mTexture->mWidth, mTexture->mHeight, 1 };

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

    mRenderer->RegisterEvent<&VkTexture::LoadToVulkan>(Events::VkGraphicsDataUpdate, this);


    vk::ComponentMapping components = { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA };

    u32 layers = 1;

    if (vk::ImageViewType::eCube == mVulkanType)
    {
      layers = 6;
    }

    vk::ImageSubresourceRange subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, layers };

    mImageView = mImage->createImageView(mVulkanType, format, components, subresourceRange);

    // 2. init sampler
    mSampler = device->createSampler(vk::Filter::eLinear,
      vk::Filter::eLinear,
      vk::SamplerMipmapMode::eLinear,
      vk::SamplerAddressMode::eRepeat,
      vk::SamplerAddressMode::eRepeat,
      vk::SamplerAddressMode::eRepeat,
      0.0f,
      true,
      16.0f,
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



  void VkTexture::LoadToVulkan(VkGraphicsDataUpdate* aEvent)
  {
    OPTICK_EVENT();

    auto update = aEvent->mCBO;
    // create a temporary upload image and fill it with pixel data. 
    // The destructor of MappedImage will put the transfer into the command buffer.
    vkhlf::MappedImage mi(mImage, update, 0, mTexture->mData.size());
    vk::SubresourceLayout layout = mi.getSubresourceLayout(vk::ImageAspectFlagBits::eColor, 0, 0);
    uint8_t* dataWriter = reinterpret_cast<uint8_t*>(mi.getPointer());

    auto const height = mTexture->mHeight;
    auto const width = mTexture->mWidth;

    u8 const* dataReader = mTexture->mData.data();

    auto const basisRowPitch = mTexture->mBytesPerBlock * ((width + 3) / 4);

    switch (mTexture->mType)
    {
      case TextureLayout::Bc1_Rgba_Srgb:
      case TextureLayout::Bc3_Srgb:
      case TextureLayout::Bc3_Unorm:
      case TextureLayout::Bc7_Unorm_Opaque:
      {        
        // Adapted from: https://stackoverflow.com/questions/36138217/unable-to-create-image-from-compressed-texture-data-s3tc
        u32 const blockSize =  mTexture->mBytesPerBlock;
        u32 const widthBlocks = width / mTexture->mBlockWidth;
        u32 const heightBlocks = height / mTexture->mBlockHeight;

        for (u32 y = 0; y < heightBlocks; ++y)
        {          
          auto* rowPtr = dataWriter + y * layout.rowPitch;

          u8 const* rowSrc = dataReader + y * basisRowPitch;

          for (u32 x = 0; x < widthBlocks; ++x)
          {
            auto* pxDest = rowPtr + x * blockSize;
            u8 const* pxSrc = rowSrc + x * blockSize;
            memcpy(pxDest, pxSrc, blockSize);
          }
        }

        break;
      }
      case TextureLayout::RGBA:
      default:
      {
        for (size_t y = 0; y < height; y++)
        {
          uint8_t* rowPtr = dataWriter;

          for (size_t x = 0; x < width; x++, rowPtr += 4, dataReader += 4)
          {
            rowPtr[0] = dataReader[0];
            rowPtr[1] = dataReader[1];
            rowPtr[2] = dataReader[2];
            rowPtr[3] = dataReader[3];
          }

          dataWriter += layout.rowPitch;
        }
      }
    }

    // Clean up CPU side texture memory.
    mTexture->mData.clear();
    mTexture->mData.shrink_to_fit();

    mRenderer->DeregisterEvent<&VkTexture::LoadToVulkan>(Events::VkGraphicsDataUpdate,  this);
  }
}
