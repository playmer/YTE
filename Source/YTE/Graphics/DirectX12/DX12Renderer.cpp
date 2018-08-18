#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Graphics/DirectX12/DX12InstantiatedModel.hpp"
#include "YTE/Graphics/DirectX12/DX12InstantiatedLight.hpp"
#include "YTE/Graphics/DirectX12/DX12InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/DirectX12/DX12Internals.hpp"
#include "YTE/Graphics/DirectX12/DX12Renderer.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"

#include "YTE/Graphics/DirectX12/DX12Texture.hpp"
#include "YTE/Graphics/DirectX12/DX12Mesh.hpp"

namespace YTE
{
  YTEDefineType(Dx12Renderer)
  {
    RegisterType<Dx12Renderer>();
    TypeBuilder<Dx12Renderer> builder;
  }

  template<typename tTo, typename tFrom>
  std::unique_ptr<tTo> static_unique_pointer_cast(std::unique_ptr<tFrom> &&aValue)
  {
    //conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
    return std::unique_ptr<tTo>{static_cast<tTo*>(aValue.release())};
  }


  Dx12Renderer::Dx12Renderer(Engine *aEngine)
    : Renderer(aEngine)
    , mVulkanInternals(std::make_unique<Dx12Internals>())
    , mEngine(aEngine)
  {
    //auto firstSurface = mVulkanInternals->InitializeVulkan(aEngine);
    //
    //// vulkan is initialized, initialize the engine
    //auto& windows = aEngine->GetWindows();
    //
    //auto instance = mVulkanInternals->GetInstance();
    //
    //
    //auto family = mVulkanInternals->GetQueueFamilies().GetGraphicsFamily();
    //vkhlf::DeviceQueueCreateInfo deviceCreate{family,
    //                                          0.0f};
    //
    //// Create a new device with the VK_KHR_SWAPCHAIN_EXTENSION enabled.
    //vk::PhysicalDeviceFeatures enabledFeatures;
    //enabledFeatures.setTextureCompressionBC(true);
    //enabledFeatures.setWideLines(true);
    //enabledFeatures.setFillModeNonSolid(true);
    //enabledFeatures.setSamplerAnisotropy(true);
    //
    //mDevice = mVulkanInternals->GetPhysicalDevice()->createDevice(deviceCreate,
    //                                                              nullptr,
    //                                                              { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
    //                                                              enabledFeatures);
    //
    //mCommandPool = mDevice->createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    //                                          mVulkanInternals->GetQueueFamilies().GetGraphicsFamily());
    //mGraphicsDataUpdateCBOB = std::make_unique<Dx12CBOB<3, false>>(mCommandPool);
    //
    //mGraphicsQueue = mDevice->getQueue(family, 0);
    //
    //mAllocators[DX12AllocatorTypes::Mesh] =
    //  std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);
    //
    //// 4x 1024 texture size for rgba in this one
    //mAllocators[DX12AllocatorTypes::Texture] =
    //  std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 4096 * 4096, nullptr);
    //
    //mAllocators[DX12AllocatorTypes::UniformBufferObject] =
    //  std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);
    //
    ////Range(std::next(windows.begin()), windows.end());
    //
    //bool firstSet = false;
    //for (auto &[name, window] : windows)
    //{
    //  if (window->mShouldBeRenderedTo)
    //  {
    //    // first window's surface is already got by Dx12Internals
    //    if (false == firstSet)
    //    {
    //      mSurfaces.emplace(window.get(),
    //                        std::make_unique<Dx12RenderedSurface>(window.get(),
    //                                                            this,
    //                                                            firstSurface)); 
    //      firstSet = true;  // disable for the next window
    //      continue;
    //    }
    //
    //    // all other windows
    //    auto surface = mVulkanInternals->CreateSurface(window.get());
    //    mSurfaces.emplace(window.get(),
    //                      std::make_unique<Dx12RenderedSurface>(window.get(),
    //                                                          this,
    //                                                          surface));
    //  }
    //}
    //
    //mEngine->RegisterEvent<&Dx12Renderer::FrameUpdate>(Events::FrameUpdate, this);
    //mEngine->RegisterEvent<&Dx12Renderer::GraphicsDataUpdate>(Events::GraphicsDataUpdate, this);
    //mEngine->RegisterEvent<&Dx12Renderer::AnimationUpdate>(Events::AnimationUpdate, this);
    //mEngine->RegisterEvent<&Dx12Renderer::PresentFrame>(Events::PresentFrame, this);
  }

  Dx12Renderer::~Dx12Renderer()
  {
    //mSurfaces.clear();
    //mTextures.clear();
    //mMeshes.clear();
  }

  void Dx12Renderer::RegisterWindowForDraw(Window* aWindow)
  {
    UnusedArguments(aWindow);
    //DebugAssert(aWindow, "Cannot create a vk rendered surface for a null window");
    //if (aWindow && aWindow->mShouldBeRenderedTo)
    //{
    //  auto surface = mVulkanInternals->CreateSurface(aWindow);
    //  mSurfaces.emplace(aWindow,
    //    std::make_unique<Dx12RenderedSurface>(aWindow,
    //      this,
    //      surface));
    //}
  }

  void Dx12Renderer::DeregisterWindowFromDraw(Window* aWindow)
  {
    mSurfaces.erase(aWindow);
  }
  
  std::unique_ptr<InstantiatedModel> Dx12Renderer::CreateModel(GraphicsView *aView,
                                                             std::string &aMeshFile)
  {
    return static_unique_pointer_cast<InstantiatedModel>(GetSurface(aView->GetWindow())->CreateModel(aView, aMeshFile));
  }

  std::unique_ptr<InstantiatedModel> Dx12Renderer::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    return static_unique_pointer_cast<InstantiatedModel>(GetSurface(aView->GetWindow())->CreateModel(aView, aMesh));
  }

  void Dx12Renderer::DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel)
  {
    GetSurface(aView->GetWindow())->DestroyMeshAndModel(aView, static_cast<DX12InstantiatedModel*>(aModel));
  }

  std::unique_ptr<InstantiatedLight> Dx12Renderer::CreateLight(GraphicsView* aView)
  {
    return static_unique_pointer_cast<InstantiatedLight>(GetSurface(aView->GetWindow())->CreateLight(aView));
  }

  std::unique_ptr<InstantiatedInfluenceMap> Dx12Renderer::CreateWaterInfluenceMap(GraphicsView* aView)
  {
    return static_unique_pointer_cast<InstantiatedInfluenceMap>(GetSurface(aView->GetWindow())->CreateWaterInfluenceMap(aView));
  }

  // Textures
  DX12Texture* Dx12Renderer::CreateTexture(std::string &aFilename/*, vk::ImageViewType aType*/)
  {
    UnusedArguments(aFilename);
    //auto textureIt = mTextures.find(aFilename);
    //DX12Texture *texturePtr{ nullptr };
    //
    //if (textureIt == mTextures.end())
    //{
    //  auto baseTexture = GetBaseTexture(aFilename);
    //
    //  auto texture = std::make_unique<DX12Texture>(baseTexture,
    //                                             this,
    //                                             aType);
    //
    //  texturePtr = texture.get();
    //  mTextures[aFilename] = std::move(texture);
    //  mDataUpdateRequired = true;
    //}
    //else
    //{
    //  texturePtr = textureIt->second.get();
    //}
    //
    //return texturePtr;
    return nullptr;
  }


  DX12Texture* Dx12Renderer::CreateTexture(std::string aName,
                                           std::vector<u8> aData,
                                           TextureLayout aType,
                                           u32 aWidth,
                                           u32 aHeight,
                                           u32 aMipLevels,
                                           u32 aLayerCount/*,
                                           vk::ImageViewType aVulkanType*/)
  {
    UnusedArguments(aName);
    UnusedArguments(aData);
    UnusedArguments(aType);
    UnusedArguments(aWidth);
    UnusedArguments(aHeight);
    UnusedArguments(aMipLevels);
    UnusedArguments(aLayerCount);
    //auto textureIt = mTextures.find(aName);
    //DX12Texture *texturePtr{ nullptr };
    //
    //if (textureIt == mTextures.end())
    //{
    //  auto baseTexture = std::make_unique<Texture>(aData, 
    //                                               aType, 
    //                                               aWidth, 
    //                                               aHeight, 
    //                                               aMipLevels, 
    //                                               aLayerCount);
    //
    //  mBaseTexturesMutex.lock();
    //
    //  auto it = mBaseTextures.find(aName);
    //
    //  if (it != mBaseTextures.end())
    //  {
    //    mBaseTextures.erase(it);
    //  }
    //
    //  auto ret = mBaseTextures.emplace(aName, std::move(baseTexture));
    //  auto baseTexturePtr = ret.first->second.get();
    //  mBaseTexturesMutex.unlock();
    //
    //
    //  auto texture = std::make_unique<DX12Texture>(baseTexturePtr,
    //                                             this,
    //                                             aVulkanType);
    //
    //  texturePtr = texture.get();
    //  mTextures[aName] = std::move(texture);
    //  mDataUpdateRequired = true;
    //}
    //else
    //{
    //  texturePtr = textureIt->second.get();
    //}
    //
    //return texturePtr;
    return nullptr;
  }

  Texture* Dx12Renderer::CreateTexture(std::string &aFilename, TextureType aType)
  {
    UnusedArguments(aType);
    //vk::ImageViewType type{ vk::ImageViewType::e2D };
    //
    //switch (aType)
    //{
    //  case TextureType::e1D: type = vk::ImageViewType::e1D; break;
    //  case TextureType::e2D: type = vk::ImageViewType::e2D; break;
    //  case TextureType::e3D: type = vk::ImageViewType::e3D; break;
    //  case TextureType::eCube: type = vk::ImageViewType::eCube; break;
    //  case TextureType::e1DArray: type = vk::ImageViewType::e1DArray; break;
    //  case TextureType::e2DArray: type = vk::ImageViewType::e2DArray; break;
    //  case TextureType::eCubeArray: type = vk::ImageViewType::eCubeArray; break;
    //}
    //
    auto texture = CreateTexture(aFilename/*, type*/);
    return texture->mTexture;
  }

  Texture* Dx12Renderer::CreateTexture(std::string aName,
                                       std::vector<u8> aData,
                                       TextureLayout aLayout,
                                       u32 aWidth,
                                       u32 aHeight,
                                       u32 aMipLevels,
                                       u32 aLayerCount,
                                       TextureType aType)
  {
    UnusedArguments(aType);
    //vk::ImageViewType type{ vk::ImageViewType::e2D };
    //
    //switch (aType)
    //{
    //  case TextureType::e1D: type = vk::ImageViewType::e1D; break;
    //  case TextureType::e2D: type = vk::ImageViewType::e2D; break;
    //  case TextureType::e3D: type = vk::ImageViewType::e3D; break;
    //  case TextureType::eCube: type = vk::ImageViewType::eCube; break;
    //  case TextureType::e1DArray: type = vk::ImageViewType::e1DArray; break;
    //  case TextureType::e2DArray: type = vk::ImageViewType::e2DArray; break;
    //  case TextureType::eCubeArray: type = vk::ImageViewType::eCubeArray; break;
    //}
    //
    auto texture = CreateTexture(aName, aData, aLayout, aWidth, aHeight, aMipLevels, aLayerCount/*, type*/);

    return texture->mTexture;
  }

  // Meshes
  DX12Mesh* Dx12Renderer::CreateMesh(std::string &aFilename)
  {
    UnusedArguments(aFilename);
    //auto baseMesh = GetBaseMesh(aFilename);
    //
    //auto meshIt = mMeshes.find(aFilename);
    //
    //DX12Mesh *meshPtr{ nullptr };
    //
    //if (meshIt == mMeshes.end())
    //{
    //  // create mesh
    //  auto mesh = std::make_unique<DX12Mesh>(baseMesh,
    //                                       this);
    //
    //  meshPtr = mesh.get();
    //
    //  mMeshes[aFilename] = std::move(mesh);
    //  mDataUpdateRequired = true;
    //}
    //else
    //{
    //  meshPtr = meshIt->second.get();
    //}
    //
    //return meshPtr;
    return nullptr;
  }
  
  Mesh* Dx12Renderer::CreateSimpleMesh(std::string &aName,
                                     std::vector<Submesh> &aSubmeshes,
		                                 bool aForceUpdate)
  {
    UnusedArguments(aName);
    UnusedArguments(aSubmeshes);
    UnusedArguments(aForceUpdate);
    //auto meshIt = mMeshes.find(aName);
    //
    //DX12Mesh *meshPtr{ nullptr };
    //
    //if (aForceUpdate || meshIt == mMeshes.end())
    //{
    //  auto baseMesh = std::make_unique<Mesh>(aName,
    //                                         aSubmeshes);
    //
    //  mBaseMeshesMutex.lock();
    //  auto it = mBaseMeshes.find(aName);
    //
    //  if (it != mBaseMeshes.end())
    //  {
    //    mBaseMeshes.erase(it);
    //  }
    //
    //  auto ret = mBaseMeshes.emplace(aName, std::move(baseMesh));
    //  auto baseMeshPtr = ret.first->second.get();
    //  mBaseMeshesMutex.unlock();
    //
    //  // create mesh
    //  auto mesh = std::make_unique<DX12Mesh>(baseMeshPtr, this);
    //
    //  auto it2 = mMeshes.find(aName);
    //
    //  if (it2 != mMeshes.end())
    //  {
    //    mMeshes.erase(it2);
    //  }
    //
    //  auto ret2 = mMeshes.emplace(aName, std::move(mesh));
    //  meshPtr = ret2.first->second.get();
    //
    //  mDataUpdateRequired = true;
    //}
    //else
    //{
    //  meshPtr = meshIt->second.get();
    //}
    //
    //return meshPtr->mMesh;
    return nullptr;
  }


  void Dx12Renderer::UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView)
  {
    GetSurface(aView->GetWindow())->UpdateSurfaceViewBuffer(aView, aUBOView);
  }



  void Dx12Renderer::UpdateWindowIlluminationBuffer(GraphicsView* aView, UBOIllumination& aIllumination)
  {
    GetSurface(aView->GetWindow())->UpdateSurfaceIlluminationBuffer(aView, aIllumination);
  }



  void Dx12Renderer::GraphicsDataUpdate(LogicUpdate *aEvent)
  {
    UnusedArguments(aEvent);
    //UnusedArguments(aEvent);
    //
    //DX12GraphicsDataUpdate update;
    //mGraphicsDataUpdateCBOB->NextCommandBuffer();
    //update.mCBO = mGraphicsDataUpdateCBOB->GetCurrentCBO();
    //
    //update.mCBO->begin();
    //
    //SendEvent(Events::DX12GraphicsDataUpdate, &update);
    //
    //update.mCBO->end();
    //
    //vkhlf::submitAndWait(mGraphicsQueue, update.mCBO);
    //
    //for (auto &surface : mSurfaces)
    //{
    //  surface.second->GraphicsDataUpdate();
    //}
  }

  void Dx12Renderer::FrameUpdate(LogicUpdate *aEvent)
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

  void Dx12Renderer::PresentFrame(LogicUpdate *aEvent)
  {
    UnusedArguments(aEvent);
    for (auto &surface : mSurfaces)
    {
      surface.second->PresentFrame();
    }
  }

  void Dx12Renderer::AnimationUpdate(LogicUpdate* aEvent)
  {
    UnusedArguments(aEvent);
    for (auto& surface : mSurfaces)
    {
      surface.second->AnimationUpdate();
    }
  }

  void Dx12Renderer::SetLights(bool aOnOrOff)
  {
    for (auto& surface : mSurfaces)
    {
      surface.second->SetLights(aOnOrOff);
    }
  }

  void Dx12Renderer::RegisterView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->RegisterView(aView);
  }

  void Dx12Renderer::RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    GetSurface(aView->GetWindow())->RegisterView(aView, aDrawerType, aCombination);
  }

  void Dx12Renderer::SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    GetSurface(aView->GetWindow())->SetViewDrawingType(aView, aDrawerType, aCombination);
  }

  void Dx12Renderer::SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination)
  {
    GetSurface(aView->GetWindow())->SetViewCombinationType(aView, aCombination);
  }

  void Dx12Renderer::DeregisterView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->DeregisterView(aView);
  }

  void Dx12Renderer::ViewOrderChanged(GraphicsView *aView, float aNewOrder)
  {
    GetSurface(aView->GetWindow())->ViewOrderChanged(aView, aNewOrder);
  }

  void Dx12Renderer::SetClearColor(GraphicsView *aView, const glm::vec4 &aColor)
  {
    GetSurface(aView->GetWindow())->SetClearColor(aView, aColor);
  }

  glm::vec4 Dx12Renderer::GetClearColor(GraphicsView *aView)
  {
    return GetSurface(aView->GetWindow())->GetClearColor(aView);
  }

  Dx12RenderedSurface* Dx12Renderer::GetSurface(Window *aWindow)
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to provided window");
      return nullptr;
    }

    return surface->second.get();
  }

  void Dx12Renderer::ResetView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->ResizeEvent(nullptr);
  }

  DX12WaterInfluenceMapManager* Dx12Renderer::GetAllWaterInfluenceMaps(GraphicsView *aView)
  {
    return &GetSurface(aView->GetWindow())->GetViewData(aView)->mWaterInfluenceMapManager;
  }

}

