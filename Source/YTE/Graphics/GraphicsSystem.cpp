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
    auto vulkanSuccess = vkelInit();

    if (vulkanSuccess)
    {
      mRenderer = static_cast<std::unique_ptr<Renderer>>(std::make_unique<VkRenderer>(mEngine));
      return;
    }

    mRenderer = std::make_unique<Renderer>();
  }
}
