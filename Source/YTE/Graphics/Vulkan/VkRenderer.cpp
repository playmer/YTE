///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"

namespace YTE
{
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
                          std::make_shared<VkRenderedSurface>(window.second.get(),
                                                              this,
                                                              surface));
      }
    }
  }



  VkRenderer::~VkRenderer()
  {
    mSurfaces.clear();
  }



  std::shared_ptr<InstantiatedModel> VkRenderer::CreateModel(Window *aWindow,
                                                             std::string &aMeshFile)
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to corresponding provided window");
    }

    return std::dynamic_pointer_cast<InstantiatedModel>(surface->second->CreateModel(aMeshFile));
  }



  void VkRenderer::DestroyModel(Window *aWindow, std::shared_ptr<InstantiatedModel> aModel)
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to corresponding provided window");
    }

    surface->second->DestroyModel(std::dynamic_pointer_cast<VkInstantiatedModel>(aModel));
  }



  void VkRenderer::UpdateWindowViewBuffer(Window *aWindow, UBOView &aView)
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to corresponding provided window");
      return;
    }

    surface->second->UpdateSurfaceViewBuffer(aView);
  }



  void VkRenderer::GraphicsDataUpdate()
  {
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



  void VkRenderer::PresentFrame()
  {
    for (auto& surface : mSurfaces)
    {
      surface.second->PresentFrame();
    }
  }



  void VkRenderer::SetClearColor(Window *aWindow, const glm::vec4 &aColor)
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to corresponding provided window");
      return;
    }

    surface->second->SetClearColor(aColor);
  }



  glm::vec4 VkRenderer::GetClearColor(Window *aWindow) const
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to corresponding provided window");
      return glm::vec4(-1.0f, -1.0f, -1.0f, -1.0f);
    }

    return surface->second->GetClearColor();
  }



  std::shared_ptr<VkRenderedSurface>& VkRenderer::GetSurface(Window *aWindow)
  {
    auto surface = mSurfaces.find(aWindow);

    if (surface == mSurfaces.end())
    {
      DebugObjection(true, "Cannot find surface associated to corresponding provided window");
      return (--surface)->second;
    }

    return surface->second;
  }
}

