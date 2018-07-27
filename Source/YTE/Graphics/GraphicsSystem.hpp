//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Graphics_GraphicsSystem_hpp
#define YTE_Graphics_GraphicsSystem_hpp

#include <stddef.h>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  YTEDeclareEvent(RendererResize);

  class RendererResize : public Event
  {
  public:
    int width;
    int height;
  };

  class GraphicsSystem : public Component
  {
  public:
    YTEDeclareType(GraphicsSystem);

    GraphicsSystem(Composition *aOwner);
    ~GraphicsSystem();

    void SetUpWindow(Window *aWindow);

    void Initialize() override;



    // Gettors
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
