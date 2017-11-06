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

    virtual ~Renderer();
    virtual std::unique_ptr<InstantiatedSprite> CreateSprite(Window *aWindow,
                                                             std::string &aTextureFile);

    virtual void DestroySprite(Window *aWindow, std::unique_ptr<InstantiatedSprite> aSprite);
    virtual std::unique_ptr<InstantiatedModel> CreateModel(Window *aWindow,
                                                           std::string &aMeshFile);

    virtual void DestroyModel(Window *aWindow, std::unique_ptr<InstantiatedModel> aModel);
    virtual void UpdateWindowViewBuffer(Window *aWindow, UBOView &aView);
    virtual void GraphicsDataUpdate(LogicUpdate *aEvent);
    virtual void FrameUpdate(LogicUpdate *aEvent);
    virtual void PresentFrame(LogicUpdate *aEvent);
    virtual glm::vec4 GetClearColor(Window *aWindow);
    virtual void SetClearColor(Window *aWindow, const glm::vec4 &aColor);
  };
}

#endif
