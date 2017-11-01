#include <filesystem>
#include <iostream>
#include <string>

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Composition.hpp"

#include "YTE/Graphics/Generics/Renderer.hpp"
#include "YTE/Graphics/Generics/Texture.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Vertex.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Platform/Window.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  YTEDefineEvent(RendererResize);

  YTEDefineType(GraphicsSystem)
  {
    YTERegisterType(GraphicsSystem);
  }



  GraphicsSystem::GraphicsSystem(Composition *aOwner, RSValue *aProperties)
    : Component(aOwner, nullptr)
    , mEngine(static_cast<Engine*>(aOwner))
    , mVulkanSuccess(0)
  {
    YTEUnusedArgument(aProperties);
  }



  GraphicsSystem::~GraphicsSystem()
  {

  }



  void GraphicsSystem::SetUpWindow(Window *aWindow)
  {
    YTEUnusedArgument(aWindow);
  }



  void GraphicsSystem::Initialize()
  {
    mEngine->YTERegister(Events::FrameUpdate, this, &GraphicsSystem::FrameUpdate);
    mEngine->YTERegister(Events::GraphicsDataUpdate, this, &GraphicsSystem::GraphicsDataUpdate);
    mEngine->YTERegister(Events::PresentFrame, this, &GraphicsSystem::PresentFrame);

    auto vulkanSuccess = vkelInit();

    if (vulkanSuccess)
    {
      mRenderer = static_cast<std::unique_ptr<Renderer>>(std::make_unique<VkRenderer>(mEngine));
      return;
    }

    mRenderer = std::make_unique<Renderer>();
  }



  void GraphicsSystem::FrameUpdate(LogicUpdate *aUpdate)
  {
    if (mRenderer)
    {
      mRenderer->FrameUpdate(aUpdate);
    }
  }



  void GraphicsSystem::GraphicsDataUpdate(LogicUpdate* aUpdate)
  {
    YTEUnusedArgument(aUpdate);
    if (mRenderer)
    {
      mRenderer->GraphicsDataUpdate();
    }
  }



  void GraphicsSystem::PresentFrame(LogicUpdate *aUpdate)
  {
    YTEUnusedArgument(aUpdate);
    if (mRenderer)
    {
      mRenderer->PresentFrame();
    }
  }
}
