///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_Renderer_hpp
#define YTE_Graphics_Generics_Renderer_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class Renderer : public EventHandler
  {
  public:
    YTEDeclareType(Renderer);

    virtual ~Renderer()
    {
      
    }

    virtual std::shared_ptr<Texture> CreateTexture(Window *aWindow, std::string &aFileName)
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aFileName);
      return nullptr;
    }


    virtual void DestroyTexture(Window *aWindow, std::shared_ptr<Texture> aTexture)
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aTexture);
    }


    virtual std::shared_ptr<InstantiatedModel> CreateModel(Window *aWindow,
      std::string &aMeshFile)
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aMeshFile);

      return nullptr;
    }

    virtual void DestroyModel(Window *aWindow, std::shared_ptr<InstantiatedModel> aModel)
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aModel);
    }

    virtual void UpdateWindowViewBuffer(Window *aWindow, UBOView &aView)
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aView);
    }

    virtual void GraphicsDataUpdate(LogicUpdate *aEvent)
    {
      YTEUnusedArgument(aEvent);
    }

    virtual void FrameUpdate(LogicUpdate *aEvent)
    {
      YTEUnusedArgument(aEvent);
    }

    virtual void PresentFrame(LogicUpdate *aEvent)
    {
      YTEUnusedArgument(aEvent);
    }

    virtual glm::vec4 GetClearColor(Window *aWindow)
    {
      YTEUnusedArgument(aWindow);
      return glm::vec4{};
    }

    virtual void SetClearColor(Window *aWindow, const glm::vec4 &aColor)
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aColor);
    }
  };
}

#endif
