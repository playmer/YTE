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
    mEngine->YTERegister(Events::PresentFrame, this, &VkRenderer::PresentFrame);
  }



  VkRenderer::~VkRenderer()
  {
    mSurfaces.clear();
  }


  std::unique_ptr<InstantiatedSprite> VkRenderer::CreateSprite(Window *aWindow, std::string &aTextureFile)
  {
    return static_unique_pointer_cast<InstantiatedSprite>(GetSurface(aWindow)->CreateSprite(aTextureFile));
  }

  void VkRenderer::DestroySprite(Window *aWindow, std::unique_ptr<InstantiatedSprite> aSprite)
  {
    GetSurface(aWindow)->DestroySprite(static_unique_pointer_cast<VkInstantiatedSprite>(std::move(aSprite)));
  }
  
  std::unique_ptr<InstantiatedModel> VkRenderer::CreateModel(Window *aWindow,
                                                             std::string &aMeshFile)
  {
    return static_unique_pointer_cast<InstantiatedModel>(GetSurface(aWindow)->CreateModel(aMeshFile));
  }

  std::unique_ptr<InstantiatedModel> VkRenderer::CreateModel(Window *aWindow, Mesh *aMesh)
  {
    return static_unique_pointer_cast<InstantiatedModel>(GetSurface(aWindow)->CreateModel(aMesh));
  }
  
  Mesh* VkRenderer::CreateSimpleMesh(Window *aWindow, 
                                     std::string &aName,
                                     std::vector<Submesh> &aSubmeshes)
  {
    return GetSurface(aWindow)->CreateSimpleMesh(aName, aSubmeshes);
  }

  void VkRenderer::UpdateWindowViewBuffer(Window *aWindow, UBOView &aView)
  {
    GetSurface(aWindow)->UpdateSurfaceViewBuffer(aView);
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



  void VkRenderer::SetClearColor(Window *aWindow, const glm::vec4 &aColor)
  {
    GetSurface(aWindow)->SetClearColor(aColor);
  }



  glm::vec4 VkRenderer::GetClearColor(Window *aWindow)
  {
    return GetSurface(aWindow)->GetClearColor();
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

