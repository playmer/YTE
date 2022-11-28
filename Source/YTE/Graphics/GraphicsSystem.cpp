#include "YTE/StandardLibrary/FileSystem.hpp"
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

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineEvent(RendererResize);

  YTEDefineType(GraphicsSystem)
  {
    RegisterType<GraphicsSystem>();
    TypeBuilder<GraphicsSystem> builder;
  }



  GraphicsSystem::GraphicsSystem(Composition *aOwner, Space*)
    : Component(aOwner, nullptr)
    , mEngine(static_cast<Engine*>(aOwner))
    , mVulkanSuccess(0)
  {
    OPTICK_EVENT();
  }



  GraphicsSystem::~GraphicsSystem()
  {

  }



  void GraphicsSystem::SetUpWindow(Window *aWindow)
  {
    UnusedArguments(aWindow);
  }



  void GraphicsSystem::Initialize()
  {
    OPTICK_EVENT();
    // TODO (Andrew): Figure out a way to choose the dummy renderer. Perhaps if an exception is thrown?
    mRenderer = static_cast<std::unique_ptr<Renderer>>(std::make_unique<VkRenderer>(mEngine));
    return;
  }
}
