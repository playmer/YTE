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
  basist::etc1_global_selector_codebook const* GetGlobalBasisCodebook()
  {
    using namespace basist;
    static basist::etc1_global_selector_codebook codebook(g_global_selector_cb_size, g_global_selector_cb);

    return &codebook;
  }


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
    Initialize();
  }

  void VkTexture::Initialize()
  {
    mTexture->mType = TextureLayout::Bc7_Unorm_Opaque;

    basist::basisu_transcoder transcoder{ GetGlobalBasisCodebook() };
    basist::basisu_file_info info;

    if (!transcoder.validate_file_checksums(
      mTexture->mData.data(),
      mTexture->mData.size(),
      true))
    {
      __debugbreak();
      return;
    }

    if (!transcoder.get_file_info(
      mTexture->mData.data(),
      mTexture->mData.size(),
      info))
    {
      __debugbreak();
      return;
    }

    if (info.m_has_alpha_slices)
    {
      mTexture->mType = TextureLayout::Bc3_Unorm;
    }

    if (
      1 != info.m_image_mipmap_levels.size() ||
      1 != info.m_total_images)
    {
      __debugbreak();
      return;
    }

    mTexture->mMipLevels = info.m_image_mipmap_levels[0];

    basist::basisu_image_info imageInfo;
    if (!transcoder.get_image_info(
      mTexture->mData.data(),
      mTexture->mData.size(),
      imageInfo,
      0))
    {
      __debugbreak();
      return;
    }

    auto constexpr transcoderFormat = basist::transcoder_texture_format::cTFBC3;
    auto const textureFormat = basist::basis_get_basisu_texture_format(transcoderFormat);

    basist::basisu_image_level_info levelInfo;

    if (!transcoder.get_image_level_info(
      mTexture->mData.data(),
      mTexture->mData.size(),
      levelInfo,
      0,
      0))
    {
      __debugbreak();
      return;
    }

    mTexture->mWidth = levelInfo.m_orig_width;
    mTexture->mHeight = levelInfo.m_orig_height;

    mTexture->mBytesPerBlock = basisu::get_bytes_per_block(textureFormat);

    mTexture->mBlockWidth = basisu::get_block_width(textureFormat);
    mTexture->mBlockHeight = basisu::get_block_height(textureFormat);

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
    basist::basisu_transcoder transcoder{ GetGlobalBasisCodebook() };
    basist::basisu_file_info info;

    auto transcoderFormat = basist::transcoder_texture_format::cTFBC7_M6_OPAQUE_ONLY;

    if (TextureLayout::Bc3_Unorm == mTexture->mType || 
        TextureLayout::Bc3_Srgb == mTexture->mType)
    {
      transcoderFormat = basist::transcoder_texture_format::cTFBC3;
    }

    auto const textureFormat = basist::basis_get_basisu_texture_format(transcoderFormat);

    auto const blocksX = (mTexture->mWidth + mTexture->mBlockWidth - 1) / mTexture->mBlockWidth;
    auto const blocksY = (mTexture->mHeight + mTexture->mBlockHeight - 1) / mTexture->mBlockHeight;

    auto const totalBlocks = blocksX * blocksY;

    auto const qwordsPerBlock = basisu::get_qwords_per_block(textureFormat);

    size_t const sizeInBytes = totalBlocks * qwordsPerBlock * sizeof(u64);

    if (!transcoder.start_transcoding(
      mTexture->mData.data(),
      mTexture->mData.size()))
    {
      __debugbreak();
      return;
    }

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
        if (!transcoder.transcode_image_level(
          mTexture->mData.data(),
          mTexture->mData.size(),
          0,
          0,
          dataWriter,
          totalBlocks,
          transcoderFormat,
          0,
          layout.rowPitch / basist::basis_get_bytes_per_block(transcoderFormat)))
        {
          __debugbreak();
          return;
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
