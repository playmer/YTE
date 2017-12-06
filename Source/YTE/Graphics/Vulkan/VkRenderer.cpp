///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/InstantiatedSprite.hpp"

#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedSprite.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"

namespace YTE
{
  YTEDefineType(VkRenderer)
  {
    YTERegisterType(VkRenderer);
  }

  template<typename tTo, typename tFrom>
  std::unique_ptr<tTo> static_unique_pointer_cast(std::unique_ptr<tFrom> &&aValue)
  {
    return std::unique_ptr<tTo>{static_cast<tTo*>(aValue.release())};
    //conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
  }


  VkRenderer::VkRenderer(Engine *aEngine)
    : mVulkanInternals(std::make_unique<VkInternals>())
    , mEngine(aEngine)
  {
    auto firstSurface = mVulkanInternals->InitializeVulkan(aEngine);

    // vulkan is initialized, initialize the engine
    auto &windows = aEngine->GetWindows();

    auto instance = mVulkanInternals->GetInstance();

    bool firstSet = false;
    for (auto &window : windows)
    {
      if (window.second->mShouldBeRenderedTo)
      {
        // first window's surface is already got by VkInternals
        if (false == firstSet)
        {
          mSurfaces.emplace(window.second.get(),
                            std::make_unique<VkRenderedSurface>(window.second.get(),
                                                                this,
                                                                firstSurface)); 
          firstSet = true;  // disable for the next window
          continue;
        }

        // all other windows
        auto surface = mVulkanInternals->CreateSurface(window.second.get());
        mSurfaces.emplace(window.second.get(),
                          std::make_unique<VkRenderedSurface>(window.second.get(),
                                                              this,
                                                              surface));
      }
    }

    mEngine->YTERegister(Events::FrameUpdate, this, &VkRenderer::FrameUpdate);
    mEngine->YTERegister(Events::GraphicsDataUpdate, this, &VkRenderer::GraphicsDataUpdate);
    mEngine->YTERegister(Events::AnimationUpdate, this, &VkRenderer::AnimationUpdate);
    mEngine->YTERegister(Events::PresentFrame, this, &VkRenderer::PresentFrame);
  }

  VkRenderer::~VkRenderer()
  {
    mSurfaces.clear();
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
  
  Mesh* VkRenderer::CreateSimpleMesh(GraphicsView *aView,
                                     std::string &aName,
                                     std::vector<Submesh> &aSubmeshes)
  {
    return GetSurface(aView->GetWindow())->CreateSimpleMesh(aName, aSubmeshes);
  }

  void VkRenderer::UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView)
  {
    GetSurface(aView->GetWindow())->UpdateSurfaceViewBuffer(aView, aUBOView);
  }

  void VkRenderer::GraphicsDataUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    for (auto& surface : mSurfaces)
    {
      surface.second->GraphicsDataUpdate();
    }
  }

  void VkRenderer::FrameUpdate(LogicUpdate *aEvent)
  {
    for (auto& surface : mSurfaces)
    {
      surface.second->FrameUpdate(aEvent);
    }
  }

  void VkRenderer::PresentFrame(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    for (auto& surface : mSurfaces)
    {
      surface.second->PresentFrame();
    }
  }

  void VkRenderer::AnimationUpdate(LogicUpdate* aEvent)
  {
    YTEUnusedArgument(aEvent);
    for (auto& surface : mSurfaces)
    {
      surface.second->AnimationUpdate();
    }
  }

  void VkRenderer::RegisterView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->RegisterView(aView);
  }

  void VkRenderer::DeregisterView(GraphicsView *aView)
  {
    GetSurface(aView->GetWindow())->DeregisterView(aView);
  }

  void VkRenderer::ViewOrderChanged(GraphicsView *aView, float aOldOrder, float aNewOrder)
  {
    GetSurface(aView->GetWindow())->ViewOrderChanged(aView, aOldOrder, aNewOrder);
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
}

