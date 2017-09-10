//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Graphics_GraphicsSystem_hpp
#define YTE_Graphics_GraphicsSystem_hpp

#include <stddef.h>

#include <functional>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/PrivateImplementation.hpp"
#include "YTE/Core/Utilities.hpp"



#include "YTE/Graphics/Renderer.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  DeclareEvent(RendererResize);

  class RendererResize : public Event
  {
  public:
    int width;
    int height;
  };

  class GraphicsSystem : public Component
  {
    public:
    DeclareType(GraphicsSystem);

    GraphicsSystem(Composition *aOwner, RSValue *aProperties);
    ~GraphicsSystem();

    void SetUpWindow(Window *aWindow);

    void Initialize();

    void Update(LogicUpdate *aUpdate);

    void SetupDrawing();

    void SetupInstanceDataBuffer();

    inline Renderer* GetRenderer()
    {
      return mRenderer.get();
    }


    PrivateImplementationDynamic mPlatformSpecificData;

    private:
    Engine *mEngine;
    std::unique_ptr<Renderer> mRenderer;


	  i32 mVulkanSuccess;
  };
}

#endif
