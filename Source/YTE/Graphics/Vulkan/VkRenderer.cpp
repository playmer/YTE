#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedLight.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"

#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"

namespace YTE
{
  VkUBOUpdates::VkUBOReference::VkUBOReference(std::shared_ptr<vkhlf::Buffer> const& aBuffer,
                                               size_t aBufferOffset,
                                               size_t aSize)
    : mBuffer(aBuffer)
    , mBufferOffset{ aBufferOffset }
    , mSize{ aSize }
  {

  }

  void VkUBOUpdates::Add(std::shared_ptr<vkhlf::Buffer> const& aBuffer,
                         u8 const* aData, 
                         size_t aSize, 
                         size_t aOffset)
  {
    std::lock_guard<std::mutex> lock(mAddingMutex);
    mReferences.emplace_back(aBuffer, aOffset, aSize);
    mData.insert(mData.end(), aData, aData + aSize);
  }

  void VkUBOUpdates::Update(std::shared_ptr<vkhlf::CommandBuffer>& aCommandBuffer)
  {
    YTEProfileFunction();
    std::lock_guard<std::mutex> lock(mAddingMutex);

    auto bytes = std::to_string(mData.size());
    auto size = mData.size();
    YTEProfileBlock(bytes.c_str());

    if (0 == size)
    {
      return;
    }

    if ((nullptr == mMappingBuffer) || size < mMappingBuffer->getSize())
    {
      auto& allocator = GetAllocator(mRenderer->GetAllocator(AllocatorTypes::BufferUpdates));
    
      mMappingBuffer = mRenderer->mDevice->createBuffer(size,
                                                        vk::BufferUsageFlagBits::eTransferSrc, 
                                                        vk::SharingMode::eExclusive, 
                                                        nullptr, 
                                                        vk::MemoryPropertyFlagBits::eHostVisible,
                                                        allocator);
    }
    
    void* pData = mMappingBuffer->get<vkhlf::DeviceMemory>()->map(0, VK_WHOLE_SIZE);
    memcpy(pData, mData.data(), size);
    auto& deviceMemory = mMappingBuffer->get<vkhlf::DeviceMemory>();
    deviceMemory->flush(0, VK_WHOLE_SIZE);
    deviceMemory->unmap();
    
    size_t dataOffset = 0;

    aCommandBuffer->getResourceTracker()->track(mMappingBuffer);

    auto commandBuffer = static_cast<vk::CommandBuffer>(*aCommandBuffer);
    
    for (auto const& reference : mReferences)
    {
      if (1 == reference.mBuffer.use_count())
      {
        dataOffset += reference.mSize;
        continue;
      }
    
      vk::BufferCopy copyOperation{ dataOffset, reference.mBufferOffset, reference.mSize };

      aCommandBuffer->getResourceTracker()->track(reference.mBuffer);
      commandBuffer.copyBuffer(*mMappingBuffer, *reference.mBuffer, copyOperation);
    
      dataOffset += reference.mSize;
    }

    mData.clear();
    mReferences.clear();
  }

  YTEDefineType(VkRenderer)
  {
    RegisterType<VkRenderer>();
    TypeBuilder<VkRenderer> builder;
  }

  template<typename tTo, typename tFrom>
  std::unique_ptr<tTo> static_unique_pointer_cast(std::unique_ptr<tFrom> &&aValue)
  {
    return std::unique_ptr<tTo>{static_cast<tTo*>(aValue.release())};
    //conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
  }


  VkRenderer::VkRenderer(Engine *aEngine)
    : Renderer{ aEngine }
    , mUBOUpdates{ this }
    , mVulkanInternals{ std::make_unique<VkInternals>() }
    , mEngine{ aEngine }
  {
    auto firstSurface = mVulkanInternals->InitializeVulkan(aEngine);

    // vulkan is initialized, initialize the engine
    auto& windows = aEngine->GetWindows();

    auto instance = mVulkanInternals->GetInstance();


    auto family = mVulkanInternals->GetQueueFamilies().GetGraphicsFamily();
    vkhlf::DeviceQueueCreateInfo deviceCreate{family,
                                              0.0f};

    // Create a new device with the VK_KHR_SWAPCHAIN_EXTENSION enabled.
    vk::PhysicalDeviceFeatures enabledFeatures;
    enabledFeatures.setTextureCompressionBC(true);
    enabledFeatures.setWideLines(true);
    enabledFeatures.setFillModeNonSolid(true);
    enabledFeatures.setSamplerAnisotropy(true);
    
    mDevice = mVulkanInternals->GetPhysicalDevice()->createDevice(deviceCreate,
                                                                  nullptr,
                                                                  { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
                                                                  enabledFeatures);

    mCommandPool = mDevice->createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                              mVulkanInternals->GetQueueFamilies().GetGraphicsFamily());
    mGraphicsDataUpdateCBOB = std::make_unique<VkCBOB<3, false>>(mCommandPool);

    mGraphicsQueue = mDevice->getQueue(family, 0);


    MakeAllocator(AllocatorTypes::Mesh, 1024 * 1024);
    MakeAllocator(AllocatorTypes::Texture, 4096 * 4096);
    MakeAllocator(AllocatorTypes::UniformBufferObject, 1024 * 1024);
    MakeAllocator(AllocatorTypes::BufferUpdates, 1024 * 1024 * 100);

    bool firstSet = false;
    for (auto &[name, window] : windows)
    {
      if (window->mShouldBeRenderedTo)
      {
        // first window's surface is already got by VkInternals
        if (false == firstSet)
        {
          mSurfaces.emplace(window.get(),
                            std::make_unique<VkRenderedSurface>(window.get(),
                                                                this,
                                                                firstSurface)); 
          firstSet = true;  // disable for the next window
          continue;
        }

        // all other windows
        auto surface = mVulkanInternals->CreateSurface(window.get());
        mSurfaces.emplace(window.get(),
                          std::make_unique<VkRenderedSurface>(window.get(),
                                                              this,
                                                              surface));
      }
    }

    mEngine->RegisterEvent<&VkRenderer::FrameUpdate>(Events::FrameUpdate, this);
    mEngine->RegisterEvent<&VkRenderer::GraphicsDataUpdate>(Events::GraphicsDataUpdate, this);
    mEngine->RegisterEvent<&VkRenderer::PresentFrame>(Events::PresentFrame, this);
  }

  VkRenderer::~VkRenderer()
  {
    mSurfaces.clear();
    mTextures.clear();
    mMeshes.clear();
  }

  void VkRenderer::RegisterWindowForDraw(Window *aWindow)
  {
    DebugAssert(aWindow, "Cannot create a vk rendered surface for a null window");
    if (aWindow && aWindow->mShouldBeRenderedTo)
    {
      auto surface = mVulkanInternals->CreateSurface(aWindow);
      mSurfaces.emplace(aWindow,
        std::make_unique<VkRenderedSurface>(aWindow,
          this,
          surface));
    }
  }

  void VkRenderer::DeregisterWindowFromDraw(Window * aWindow)
  {
    mSurfaces.erase(aWindow);
  }
  
  std::unique_ptr<InstantiatedModel> VkRenderer::CreateModel(GraphicsView *aView,
                                                             std::string &aMeshFile)
  {
    return static_unique_pointer_cast<InstantiatedModel>(GetSurface(aView->GetWindow())->CreateModel(aView, aMeshFile));
  }

  std::unique_ptr<InstantiatedModel> VkRenderer::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    return static_unique_pointer_cast<InstantiatedModel>(GetSurface(aView->GetWindow())->CreateModel(aView, aMesh));
  }

  void VkRenderer::DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel)
  {
    GetSurface(aView->GetWindow())->DestroyMeshAndModel(aView, static_cast<VkInstantiatedModel*>(aModel));
  }

  std::unique_ptr<InstantiatedLight> VkRenderer::CreateLight(GraphicsView* aView)
  {
    return static_unique_pointer_cast<InstantiatedLight>(GetSurface(aView->GetWindow())->CreateLight(aView));
  }

  std::unique_ptr<InstantiatedInfluenceMap> VkRenderer::CreateWaterInfluenceMap(GraphicsView* aView)
  {
    return static_unique_pointer_cast<InstantiatedInfluenceMap>(GetSurface(aView->GetWindow())->CreateWaterInfluenceMap(aView));
  }

  // Textures
  VkTexture* VkRenderer::CreateTexture(std::string &aFilename, vk::ImageViewType aType)
  {
    auto textureIt = mTextures.find(aFilename);
    VkTexture *texturePtr{ nullptr };

    if (textureIt == mTextures.end())
    {
      auto baseTexture = GetBaseTexture(aFilename);

      auto texture = std::make_unique<VkTexture>(baseTexture,
                                                 this,
                                                 aType);

      texturePtr = texture.get();
      mTextures[aFilename] = std::move(texture);
      mDataUpdateRequired = true;
    }
    else
    {
      texturePtr = textureIt->second.get();
    }

    return texturePtr;
  }


  VkTexture* VkRenderer::CreateTexture(std::string aName,
                                       std::vector<u8> aData,
                                       TextureLayout aType,
                                       u32 aWidth,
                                       u32 aHeight,
                                       u32 aMipLevels,
                                       u32 aLayerCount,
                                       vk::ImageViewType aVulkanType)
  {
    auto textureIt = mTextures.find(aName);
    VkTexture *texturePtr{ nullptr };

    if (textureIt == mTextures.end())
    {
      auto baseTexture = std::make_unique<Texture>(aData, 
                                                   aType, 
                                                   aWidth, 
                                                   aHeight, 
                                                   aMipLevels, 
                                                   aLayerCount);

      mBaseTexturesMutex.lock();

      auto it = mBaseTextures.find(aName);

      if (it != mBaseTextures.end())
      {
        mBaseTextures.erase(it);
      }

      auto ret = mBaseTextures.emplace(aName, std::move(baseTexture));
      auto baseTexturePtr = ret.first->second.get();
      mBaseTexturesMutex.unlock();


      auto texture = std::make_unique<VkTexture>(baseTexturePtr,
                                                 this,
                                                 aVulkanType);

      texturePtr = texture.get();
      mTextures[aName] = std::move(texture);
      mDataUpdateRequired = true;
    }
    else
    {
      texturePtr = textureIt->second.get();
    }

    return texturePtr;
  }

  vk::ImageViewType ToVkType(TextureType aTextureType)
  {
    switch (aTextureType)
    {
      case TextureType::e1D: return vk::ImageViewType::e1D; break;
      case TextureType::e2D: return  vk::ImageViewType::e2D; break;
      case TextureType::e3D: return  vk::ImageViewType::e3D; break;
      case TextureType::eCube: return  vk::ImageViewType::eCube; break;
      case TextureType::e1DArray: return  vk::ImageViewType::e1DArray; break;
      case TextureType::e2DArray: return  vk::ImageViewType::e2DArray; break;
      case TextureType::eCubeArray: return  vk::ImageViewType::eCubeArray; break;
    }

    DebugAssert(false, "Bad value passed");
    return vk::ImageViewType::e1D;
  }

  Texture* VkRenderer::CreateTexture(std::string &aFilename, TextureType aType)
  {
    auto texture = CreateTexture(aFilename, ToVkType(aType));
    return texture->mTexture;
  }

  Texture* VkRenderer::CreateTexture(std::string aName,
                                   std::vector<u8> aData,
                                   TextureLayout aLayout,
                                   u32 aWidth,
                                   u32 aHeight,
                                   u32 aMipLevels,
                                   u32 aLayerCount,
                                   TextureType aType)
  {
    auto texture = CreateTexture(aName, aData, aLayout, aWidth, aHeight, aMipLevels, aLayerCount, ToVkType(aType));

    return texture->mTexture;
  }

  // Meshes
  VkMesh* VkRenderer::CreateMesh(std::string &aFilename)
  {
    auto baseMesh = GetBaseMesh(aFilename);

    auto meshIt = mMeshes.find(aFilename);

    VkMesh *meshPtr{ nullptr };

    if (meshIt == mMeshes.end())
    {
      // create mesh
      auto mesh = std::make_unique<VkMesh>(baseMesh,
                                           this);

      meshPtr = mesh.get();

      mMeshes[aFilename] = std::move(mesh);
      mDataUpdateRequired = true;
    }
    else
    {
      meshPtr = meshIt->second.get();
    }

    return meshPtr;
  }
  
  Mesh* VkRenderer::CreateSimpleMesh(std::string &aName,
                                     ContiguousRange<SubmeshData> aSubmeshes,
		                                 bool aForceUpdate)
  {
    auto meshIt = mMeshes.find(aName);

    VkMesh *meshPtr{ nullptr };

    if (aForceUpdate || meshIt == mMeshes.end())
    {
      auto baseMesh = std::make_unique<Mesh>(this, aName, aSubmeshes);

      mBaseMeshesMutex.lock();
      auto it = mBaseMeshes.find(aName);

      if (it != mBaseMeshes.end())
      {
        mBaseMeshes.erase(it);
      }

      auto ret = mBaseMeshes.emplace(aName, std::move(baseMesh));
      auto baseMeshPtr = ret.first->second.get();
      mBaseMeshesMutex.unlock();

      // create mesh
      auto mesh = std::make_unique<VkMesh>(baseMeshPtr, this);

      auto it2 = mMeshes.find(aName);

      if (it2 != mMeshes.end())
      {
        mMeshes.erase(it2);
      }

      auto ret2 = mMeshes.emplace(aName, std::move(mesh));
      meshPtr = ret2.first->second.get();

      mDataUpdateRequired = true;
    }
    else
    {
      meshPtr = meshIt->second.get();
    }

    return meshPtr->mMesh;
  }

  void VkRenderer::UpdateWindowViewBuffer(GraphicsView *aView, UBOs::View &aUBOView)
  {
    GetSurface(aView->GetWindow())->UpdateSurfaceViewBuffer(aView, aUBOView);
  }
   
  void VkRenderer::UpdateWindowIlluminationBuffer(GraphicsView* aView, UBOs::Illumination& aIllumination)
  {
    GetSurface(aView->GetWindow())->UpdateSurfaceIlluminationBuffer(aView, aIllumination);
  }

  void VkRenderer::GraphicsDataUpdate(LogicUpdate *aEvent)
  {
    UnusedArguments(aEvent);

    VkGraphicsDataUpdate update;
    mGraphicsDataUpdateCBOB->NextCommandBuffer();
    update.mCBO = mGraphicsDataUpdateCBOB->GetCurrentCBO();

    update.mCBO->begin();

    // Currently the VkLightManager relies on this being sent, should have them do something else.
    SendEvent(Events::VkGraphicsDataUpdate, &update);
    mUBOUpdates.Update(update.mCBO);

    update.mCBO->end();

    vkhlf::submitAndWait(mGraphicsQueue, update.mCBO);

    for (auto &surface : mSurfaces)
    {
      surface.second->GraphicsDataUpdate();
    }
  }

  void VkRenderer::FrameUpdate(LogicUpdate *aEvent)
  {
    YTEProfileFunction();

    if (mDataUpdateRequired)
    {
      GraphicsDataUpdate(aEvent);
    }

    for (auto& surface : mSurfaces)
    {
      surface.second->FrameUpdate(aEvent);
    }
  }

  void VkRenderer::PresentFrame(LogicUpdate *aEvent)
  {
    UnusedArguments(aEvent);
    for (auto &surface : mSurfaces)
    {
      surface.second->PresentFrame();
    }
  }

  void VkRenderer::SetLights(bool aOnOrOff)
  {
    for (auto& surface : mSurfaces)
    {
      surface.second->SetLights(aOnOrOff);
    }
  }

  void VkRenderer::RegisterView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->RegisterView(aView);
  }

  void VkRenderer::RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    GetSurface(aView->GetWindow())->RegisterView(aView, aDrawerType, aCombination);
  }

  void VkRenderer::SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    GetSurface(aView->GetWindow())->SetViewDrawingType(aView, aDrawerType, aCombination);
  }

  void VkRenderer::SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination)
  {
    GetSurface(aView->GetWindow())->SetViewCombinationType(aView, aCombination);
  }

  void VkRenderer::DeregisterView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->DeregisterView(aView);
  }

  void VkRenderer::ViewOrderChanged(GraphicsView *aView, float aNewOrder)
  {
    GetSurface(aView->GetWindow())->ViewOrderChanged(aView, aNewOrder);
  }

  void VkRenderer::SetClearColor(GraphicsView *aView, const glm::vec4 &aColor)
  {
    GetSurface(aView->GetWindow())->SetClearColor(aView, aColor);
  }

  glm::vec4 VkRenderer::GetClearColor(GraphicsView *aView)
  {
    return GetSurface(aView->GetWindow())->GetClearColor(aView);
  }

  VkRenderedSurface* VkRenderer::GetSurface(Window *aWindow)
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to provided window");
      return nullptr;
    }

    return surface->second.get();
  }

  void VkRenderer::ResetView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->ResizeEvent(nullptr);
  }

  VkWaterInfluenceMapManager* VkRenderer::GetAllWaterInfluenceMaps(GraphicsView *aView)
  {
    return &GetSurface(aView->GetWindow())->GetViewData(aView)->mWaterInfluenceMapManager;
  }


  GPUAllocator* VkRenderer::MakeAllocator(std::string const& aAllocatorType, size_t aBlockSize)
  {
    auto allocator = std::make_unique<VkGPUAllocator>(aBlockSize, this);
    auto toReturn = allocator.get();

    mAllocators.emplace(aAllocatorType, std::move(allocator));

    return toReturn;
  }

  VkGPUAllocator::VkGPUAllocator(size_t aBlockSize, VkRenderer* aRenderer)
    : GPUAllocator{aBlockSize}
  {
    auto device = aRenderer->mDevice;
    auto allocator = std::make_shared<vkhlf::DeviceMemoryAllocator>(device, aBlockSize, nullptr);
    mData.ConstructAndGet<VkGPUAllocatorData>(allocator, device, aRenderer);
  }

  template <typename tType>
  u64 ToU64(tType aValue)
  {
    return static_cast<u64>(aValue);
  }

  vk::MemoryPropertyFlags ToVulkan(GPUAllocation::MemoryProperty aValue)
  {
    vk::MemoryPropertyFlags toReturn{};

    auto value = ToU64(aValue);

    if (0 != (value & ToU64(GPUAllocation::MemoryProperty::DeviceLocal)))
    {
      toReturn = toReturn | vk::MemoryPropertyFlagBits::eDeviceLocal;
    }
    if (0 != (value & ToU64(GPUAllocation::MemoryProperty::HostVisible)))
    {
      toReturn = toReturn | vk::MemoryPropertyFlagBits::eHostVisible;
    }
    if (0 != (value & ToU64(GPUAllocation::MemoryProperty::HostCoherent)))
    {
      toReturn = toReturn | vk::MemoryPropertyFlagBits::eHostCoherent;
    }
    if (0 != (value & ToU64(GPUAllocation::MemoryProperty::HostCached)))
    {
      toReturn = toReturn | vk::MemoryPropertyFlagBits::eHostCached;
    }
    if (0 != (value & ToU64(GPUAllocation::MemoryProperty::LazilyAllocated)))
    {
      toReturn = toReturn | vk::MemoryPropertyFlagBits::eLazilyAllocated;
    }
    if (0 != (value & ToU64(GPUAllocation::MemoryProperty::Protected)))
    {
      toReturn = toReturn | vk::MemoryPropertyFlagBits::eProtected;
    }

    return toReturn;
  }

  vk::BufferUsageFlags ToVulkan(GPUAllocation::BufferUsage aValue)
  {
    vk::BufferUsageFlags toReturn{};

    auto value = ToU64(aValue);

    if (0 != (value & ToU64(GPUAllocation::BufferUsage::TransferSrc)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eTransferSrc;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::TransferDst)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eTransferDst;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::UniformTexelBuffer)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eUniformTexelBuffer;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::StorageTexelBuffer)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eStorageTexelBuffer;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::UniformBuffer)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eUniformBuffer;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::StorageBuffer)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eStorageBuffer;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::IndexBuffer)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eIndexBuffer;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::VertexBuffer)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eVertexBuffer;
    }
    if (0 != (value & ToU64(GPUAllocation::BufferUsage::IndirectBuffer)))
    {
      toReturn = toReturn | vk::BufferUsageFlagBits::eIndirectBuffer;
    }

    return toReturn;
  }

  std::unique_ptr<GPUBufferBase> VkGPUAllocator::CreateBufferInternal(size_t aSize, 
                                                                      GPUAllocation::BufferUsage aUsage, 
                                                                      GPUAllocation::MemoryProperty aProperties)
  {
    auto self = mData.Get<VkGPUAllocatorData>();

    auto base = std::make_unique<VkUBO>(aSize);
    
    auto uboData = base->GetData().ConstructAndGet<VkUBOData>();

    auto usage = ToVulkan(aUsage);
    auto properties = ToVulkan(aProperties);
    
    uboData->mBuffer = self->mDevice->createBuffer(aSize,
                                                   ToVulkan(aUsage),
                                                   vk::SharingMode::eExclusive,
                                                   nullptr,
                                                   ToVulkan(aProperties),
                                                   self->mAllocator);

    uboData->mRenderer = self->mRenderer;

    return static_unique_pointer_cast<GPUBufferBase>(std::move(base));
  }
}

