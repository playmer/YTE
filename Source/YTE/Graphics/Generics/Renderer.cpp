///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/InstantiatedLight.hpp"


#include "YTE/Graphics/Generics/Renderer.hpp"

namespace YTE
{
  YTEDefineType(Renderer)
  {
    YTERegisterType(Renderer);
  }


  Renderer::~Renderer()
  {

  }

  void Renderer::RegisterWindowForDraw(Window *aWindow)
  {
    YTEUnusedArgument(aWindow);
  }

  void Renderer::DeregisterWindowFromDraw(Window * aWindow)
  {
    YTEUnusedArgument(aWindow);
  }

  std::unique_ptr<InstantiatedModel> Renderer::CreateModel(GraphicsView *aView,
                                                           std::string &aMeshFile)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aMeshFile);

    return nullptr;
  }

  std::unique_ptr<InstantiatedModel> Renderer::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aMesh);
    return nullptr;
  }

  Mesh* Renderer::CreateSimpleMesh(GraphicsView *aView, std::string &aName, std::vector<Submesh> &aSubmeshes)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aName);
    YTEUnusedArgument(aSubmeshes);

    return nullptr;
  }

  std::unique_ptr<InstantiatedLight> Renderer::CreateLight(GraphicsView* aView)
  {
    YTEUnusedArgument(aView);
    return nullptr;
  }



  void Renderer::UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aUBOView);
  }

  void Renderer::UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOIllumination &aIllumination)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aIllumination);
  }


  void Renderer::GraphicsDataUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void Renderer::FrameUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void Renderer::PresentFrame(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  glm::vec4 Renderer::GetClearColor(GraphicsView *aView)
  {
    YTEUnusedArgument(aView);
    return glm::vec4{};
  }

  void Renderer::SetClearColor(GraphicsView *aView, const glm::vec4 &aColor)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aColor);
  }

  void Renderer::AnimationUpdate(LogicUpdate* aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void Renderer::SetLights(bool aOnOrOff)
  {
    YTEUnusedArgument(aOnOrOff);
  }

  void Renderer::RegisterView(GraphicsView *aView)
  {
    YTEUnusedArgument(aView);
  }

  void Renderer::DeregisterView(GraphicsView *aView)
  {
    YTEUnusedArgument(aView);
  }

  void Renderer::ViewOrderChanged(GraphicsView *aView, float aOldOrder, float aNewOrder)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aOldOrder);
    YTEUnusedArgument(aNewOrder);
  }
}
