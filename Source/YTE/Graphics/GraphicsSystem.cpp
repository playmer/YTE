#include <iostream>
#include <filesystem>
#include <string>

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Composition.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/ShaderDescriptions.hpp"
#include "YTE/Graphics/Texture.hpp"
#include "YTE/Graphics/Vertex.hpp"

#include "YTE/Graphics/Vulkan/VkPrimitives.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

#include "YTE/Physics/Transform.h"

#include "YTE/Platform/Window.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  DefineType(GraphicsSystem)
  {
    YTERegisterType(GraphicsSystem);
  }

  GraphicsSystem::GraphicsSystem(Composition *aOwner, RSValue *aProperties)
    : Component(aOwner, nullptr), 
      mEngine(static_cast<Engine*>(aOwner)), 
      mVulkanSuccess(0)
  {
  }

  GraphicsSystem::~GraphicsSystem()
  {
    if (mVulkanSuccess)
    {
    }
  }

  void GraphicsSystem::SetUpWindow(Window *aWindow)
  {
  }

  void GraphicsSystem::Initialize()
  {
    mEngine->RegisterListener(Events::FrameUpdate, *this, &GraphicsSystem::Update);

    auto vulkanSuccess = vkelInit();

    if (vulkanSuccess)
    {
      mRenderer = static_cast<std::unique_ptr<Renderer>>(std::make_unique<VkRenderer>(mEngine));
      return;
    }

    mRenderer = std::make_unique<Renderer>();
  }

  void GraphicsSystem::SetupDrawing()
  {
  }

  void GraphicsSystem::SetupInstanceDataBuffer()
  {
  }

  void GraphicsSystem::Update(LogicUpdate *aUpdate)
  {
  }
}
