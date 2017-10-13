
#include <array>
#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/PrivateImplementation.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/ShaderDescriptions.hpp"
#include "YTE/Graphics/Texture.hpp"
#include "YTE/Graphics/View.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

#include "YTE/Utilities/Utilities.h"


namespace fs = std::experimental::filesystem;

namespace YTE
{
  void InstantiatedMeshRendererData::UpdateUniformBuffer(InstantiatedMesh & aModel)
  {
    UBOModel model;

    model.mModelMatrix = glm::translate(model.mModelMatrix, aModel.mPosition);
    model.mModelMatrix = model.mModelMatrix * glm::toMat4(aModel.mRotation);
    model.mModelMatrix = glm::scale(model.mModelMatrix, aModel.mScale);

    auto update = mSurface->mCommandPool->allocateCommandBuffer();

    update->begin();
    mUBOModel->update<UBOModel>(0, model, update);
    update->end();

    vkhlf::submitAndWait(mSurface->mGraphicsQueue, update);
  }

  VkRenderer::~VkRenderer()
  {
    vkelUninit();
  }

  VkRenderer::VkRenderer(Engine *aEngine)
    : mEngine(aEngine)
  {
    std::vector<std::string>  enabledExtensions, enabledLayers;

    enabledExtensions.emplace_back("VK_KHR_surface");

#if defined(Windows)
    enabledExtensions.emplace_back("VK_KHR_win32_surface");
#endif

#if !defined(NDEBUG)
    enabledExtensions.emplace_back("VK_EXT_debug_report");

    // Enable standard validation layer to find as much errors as possible!
    enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	
	//enabledLayers.push_back("VK_LAYER_GOOGLE_threading");
	//enabledLayers.push_back("VK_LAYER_GOOGLE_unique_objects");
	//enabledLayers.push_back("VK_LAYER_LUNARG_api_dump");
	//enabledLayers.push_back("VK_LAYER_LUNARG_device_limits");
	//enabledLayers.push_back("VK_LAYER_LUNARG_draw_state");
	//enabledLayers.push_back("VK_LAYER_LUNARG_image");
	//enabledLayers.push_back("VK_LAYER_LUNARG_mem_tracker");
	//enabledLayers.push_back("VK_LAYER_LUNARG_object_tracker");
	//enabledLayers.push_back("VK_LAYER_LUNARG_param_checker");
	//enabledLayers.push_back("VK_LAYER_LUNARG_screenshot");
	//enabledLayers.push_back("VK_LAYER_LUNARG_swapchain");
#endif

    // Create a new vulkan instance using the required extensions
    mInstance = vkhlf::Instance::create("Yours Truly Engine",
      1,
      enabledLayers,
      enabledExtensions);

#if !defined(NDEBUG)
    vk::DebugReportFlagsEXT flags(//vk::DebugReportFlagBitsEXT::eInformation        |
      vk::DebugReportFlagBitsEXT::eWarning |
      vk::DebugReportFlagBitsEXT::ePerformanceWarning |
      vk::DebugReportFlagBitsEXT::eError |
      vk::DebugReportFlagBitsEXT::eDebug);
    mDebugReportCallback
      = mInstance->createDebugReportCallback(flags,
        &debugReportCallback);
#endif

    vkelInstanceInit(static_cast<vk::Instance>(*mInstance));

    auto &windows = aEngine->GetWindows();

    for (auto &window : windows)
    {
      if (window.second->mShouldBeRenderedTo)
      {
        auto surface = window.second->SetUpVulkanWindow(static_cast<void*>(mInstance.get()));
        mSurfaces.emplace(window.second.get(),
          std::make_unique<RenderedSurface>(window.second.get(),
            this,
            surface,
            mInstance));
      }
    }
  }

  Texture* VkRenderer::AddTexture(Window *aWindow,
    const char *aTextureFile)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return nullptr;
    }

    auto surface = surfaceIt->second.get();

    return AddTexture(surface, aTextureFile);
  };

  Texture* VkRenderer::AddTexture(RenderedSurface *aSurface,
    const char *aTexture)
  {
    auto device = aSurface->mDevice;

    fs::path file{ aTexture };

    std::string textureName{ file.stem().string() };

    //TODO (Josh): Make Crunch work.
    //file = L"Crunch" / file.filename().concat(L".crn");
    file = L"Originals" / file.filename();
    std::string fileStr{ file.string() };

    auto &textureMap = mTextures[device.get()];

    auto it = textureMap.find(textureName);

    if (it != textureMap.end())
    {
      return it->second.get();
    }

    auto textureIt = textureMap.emplace(textureName, std::make_unique<Texture>(fileStr));

    auto texture = textureIt.first->second.get();

    auto rendererData = texture->mRendererData.ConstructAndGet<TextureRendererData>();

    auto allocator = mAllocators[device.get()][AllocatorTypes::Texture];

    auto update = aSurface->mCommandPool->allocateCommandBuffer();

    // 1. init image
    vk::Format format;

    switch (texture->mType)
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
    }

    vk::FormatProperties imageFormatProperties = aSurface->mMainDevice->getFormatProperties(format);

    assert((imageFormatProperties.linearTilingFeatures &
      vk::FormatFeatureFlagBits::eSampledImage) ||
      (imageFormatProperties.optimalTilingFeatures &
        vk::FormatFeatureFlagBits::eSampledImage));

    vk::Extent3D imageExtent{ texture->mWidth, texture->mHeight, 1 };

    rendererData->mImage = device->createImage({},
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

    update->begin();
    {
      // create a temporary upload image and fill it with pixel data. 
      // The destructor of MappedImage will put the transfer into the command buffer.
      vkhlf::MappedImage mi(rendererData->mImage, update, 0, texture->mData.size());
      vk::SubresourceLayout layout = mi.getSubresourceLayout(vk::ImageAspectFlagBits::eColor, 0, 0);
      uint8_t * data = reinterpret_cast<uint8_t*>(mi.getPointer());

      auto height = texture->mHeight;
      auto width = texture->mWidth;

      auto pixels = texture->mData.data();

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
    }
    update->end();
    vkhlf::submitAndWait(aSurface->mGraphicsQueue, update);

    rendererData->mView = rendererData->mImage->createImageView(vk::ImageViewType::e2D, format);

    // 2. init sampler
    rendererData->mSampler = device->createSampler(vk::Filter::eNearest,
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


    return texture;
  }

  Mesh* VkRenderer::AddMesh(RenderedSurface *aSurface,
    std::string &aFilename)
  {
    auto update = aSurface->mCommandPool->allocateCommandBuffer();
    auto device = aSurface->mDevice;

    auto &meshMap = mMeshes[device.get()];

    auto it = meshMap.find(aFilename);

    if (it != meshMap.end())
    {
      return it->second.get();
    }

    auto meshIt = meshMap.emplace(aFilename,
      std::make_unique<Mesh>(aSurface->mRenderer,
        aSurface->mWindow,
        aFilename));

    auto mesh = meshIt.first->second.get();

    auto allocator = mAllocators[device.get()][AllocatorTypes::Mesh];

    for (auto &subMesh : mesh->mParts)
    {
      auto subMeshData = subMesh.mRendererData.ConstructAndGet<MeshRendererData>();

      subMeshData->mVertexBuffer =
        aSurface->mDevice->createBuffer(subMesh.mVertexBufferSize,
          vk::BufferUsageFlagBits::eTransferDst |
          vk::BufferUsageFlagBits::eVertexBuffer,
          vk::SharingMode::eExclusive,
          nullptr,
          vk::MemoryPropertyFlagBits::eDeviceLocal,
          allocator);

      subMeshData->mIndexBuffer =
        aSurface->mDevice->createBuffer(subMesh.mIndexBufferSize,
          vk::BufferUsageFlagBits::eTransferDst |
          vk::BufferUsageFlagBits::eIndexBuffer,
          vk::SharingMode::eExclusive,
          nullptr,
          vk::MemoryPropertyFlagBits::eDeviceLocal,
          allocator);

      subMeshData->mUBOMaterial =
        aSurface->mDevice->createBuffer(sizeof(UBOMaterial),
          vk::BufferUsageFlagBits::eTransferDst |
          vk::BufferUsageFlagBits::eUniformBuffer,
          vk::SharingMode::eExclusive,
          nullptr,
          vk::MemoryPropertyFlagBits::eDeviceLocal,
          allocator);

      auto update = aSurface->mCommandPool->allocateCommandBuffer();

      update->begin();
      subMeshData->mVertexBuffer->update<Vertex>(0, subMesh.mVertexBuffer, update);
      subMeshData->mIndexBuffer->update<u32>(0, subMesh.mIndexBuffer, update);
      subMeshData->mUBOMaterial->update<UBOMaterial>(0, subMesh.mUBOMaterial, update);
      update->end();

      vkhlf::submitAndWait(aSurface->mGraphicsQueue, update);
    }

    return mesh;
  }

  void VkRenderer::RemoveMeshId(Window *aWindow, u64 aId)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return;
    }

    for (auto &pipelineIt : surfaceIt->second->mPipelines)
    {
      auto &models = pipelineIt.second.mModels;

      auto it = models.Find(aId);

      if (it != models.end())
      {
        models.Erase(it);
      }
    }
  }

  glm::vec4 VkRenderer::GetClearColor(Window *aWindow)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return glm::vec4{};
    }

    return surfaceIt->second->mClearColor;
  }

  void VkRenderer::SetClearColor(Window *aWindow, const glm::vec4 &aColor)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return;
    }

    surfaceIt->second->mClearColor = aColor;
  }

  void VkRenderer::UpdateViewBuffer(Window *aWindow, UBOView &aView)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return;
    }

    surfaceIt->second->UpdateViewBuffer(aView);
  }
}
