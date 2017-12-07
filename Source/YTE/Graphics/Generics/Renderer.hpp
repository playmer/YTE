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
    virtual void DeregisterWindowFromDraw(Window *aWindow);
    virtual void RegisterWindowForDraw(Window *aWindow);
    virtual std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView,
                                                           std::string &aMeshFile);
    virtual std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, Mesh *aMesh);

    virtual Mesh* CreateSimpleMesh(GraphicsView *aView,
                                   std::string &aName,
                                   std::vector<Submesh> &aSubmeshes);
    virtual std::unique_ptr<InstantiatedLight> CreateLight(GraphicsView *aView);

    virtual void UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView);
    virtual void UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOIllumination &aIllumination);
    virtual void GraphicsDataUpdate(LogicUpdate *aEvent);
    virtual void FrameUpdate(LogicUpdate *aEvent);
    virtual void PresentFrame(LogicUpdate *aEvent);
    virtual glm::vec4 GetClearColor(GraphicsView *aView);
    virtual void SetClearColor(GraphicsView *aView, const glm::vec4 &aColor);
    virtual void AnimationUpdate(LogicUpdate *aEvent);

    virtual void RegisterView(GraphicsView *aView);
    virtual void DeregisterView(GraphicsView *aView);
    virtual void ViewOrderChanged(GraphicsView *aView, float aOldOrder, float aNewOrder);
  };
}

#endif
