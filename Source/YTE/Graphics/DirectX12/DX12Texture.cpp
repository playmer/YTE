#include <array>
#include <filesystem>
#include <fstream>

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/DirectX12/DX12Internals.hpp"
#include "YTE/Graphics/DirectX12/DX12Renderer.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Texture.hpp"
#include "YTE/Graphics/DirectX12/DX12DeviceInfo.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace fs = std::experimental::filesystem;

namespace YTE
{
  YTEDefineType(DX12Texture)
  {
    RegisterType<DX12Texture>();
    TypeBuilder<DX12Texture> builder;
  }


  DX12Texture::DX12Texture(Texture *aTexture,
                       Dx12Renderer *aRenderer,
                       vk::ImageViewType aVulkanType)
    : mRenderer{ aRenderer }
    , mVulkanType{ aVulkanType }
    , mTexture{ aTexture }
  {
    Initialize();
  }

  void DX12Texture::Initialize()
  {
    auto device = mRenderer->mDevice;

    auto allocator = mRenderer->mAllocators[AllocatorTypes::Texture];

    // 1. init image
    vk::Format format;

    switch (mTexture->mType)
    {
      case TextureLayout::DXT1_sRGB:
      {
        format = vk::Format::eBc1RgbaSrgbBlock;
        break;
      }
      case TextureLayout::DXT5_sRGB:
      {
        format = vk::Format::eBc3UnormBlock;
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
      mRenderer->GetDx12Internals()->GetPhysicalDevice()->getFormatProperties(format);

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

    mRenderer->RegisterEvent<&DX12Texture::LoadToVulkan>(Events::DX12GraphicsDataUpdate, this);


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



  DX12Texture::~DX12Texture()
  {
  }



  void DX12Texture::LoadToVulkan(DX12GraphicsDataUpdate *aEvent)
  {
    auto update = aEvent->mCBO;
    // create a temporary upload image and fill it with pixel data. 
    // The destructor of MappedImage will put the transfer into the command buffer.
    vkhlf::MappedImage mi(mImage, update, 0, mTexture->mData.size());
    vk::SubresourceLayout layout = mi.getSubresourceLayout(vk::ImageAspectFlagBits::eColor, 0, 0);
    uint8_t *data = reinterpret_cast<uint8_t*>(mi.getPointer());

    auto height = mTexture->mHeight;
    auto width = mTexture->mWidth;

    auto pixels = mTexture->mData.data();

    switch (mTexture->mType)
    {
      case TextureLayout::DXT1_sRGB:
      {
        break;
      }
      case TextureLayout::DXT5_sRGB:
      {
        // Adapted from: https://stackoverflow.com/questions/36138217/unable-to-create-image-from-compressed-texture-data-s3tc
        constexpr u32 blockSize{ 16 };
        u32 widthBlocks = width / 4;
        u32 heightBlocks = height / 4;
        for (u32 y = 0; y < heightBlocks; ++y)
        {
          auto *rowPtr = data + y * layout.rowPitch; // rowPitch is 0
          auto *rowSrc = pixels + y * (widthBlocks * blockSize);
          for (u32 x = 0; x < widthBlocks; ++x)
          {
            auto *pxDest = rowPtr + x * blockSize;
            auto *pxSrc = rowSrc + x * blockSize; // 4x4 image block
            memcpy(pxDest, pxSrc, blockSize); // 128Bit per block
          }
        }

        break;
      }
      case TextureLayout::RGBA:
      default:
      {
        for (size_t y = 0; y < height; y++)
        {
          uint8_t *rowPtr = data;
          for (size_t x = 0; x < width; x++, rowPtr += 4, pixels += 4)
          {
            rowPtr[0] = pixels[0];
            rowPtr[1] = pixels[1];
            rowPtr[2] = pixels[2];
            rowPtr[3] = pixels[3];
          }
          data += layout.rowPitch;
        }
      }
    }

    // TODO (JoshF): Please think of a better way to unload texture data.
    //mTexture->mData.clear();
    //mTexture->mData.shrink_to_fit();

    mRenderer->DeregisterEvent<&DX12Texture::LoadToVulkan>(Events::DX12GraphicsDataUpdate,  this);
  }
}
