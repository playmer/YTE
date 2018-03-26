///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/InstantiatedLight.hpp"
#include "YTE/Graphics/Generics/InstantiatedInfluenceMap.hpp"


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

  void Renderer::DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aModel);
  }

  Mesh* Renderer::CreateSimpleMesh(std::string &aName, std::vector<Submesh> &aSubmeshes, bool aForceUpdate)
  {
    YTEUnusedArgument(aName);
    YTEUnusedArgument(aSubmeshes);
		YTEUnusedArgument(aForceUpdate);

    return nullptr;
  }

  std::unique_ptr<InstantiatedLight> Renderer::CreateLight(GraphicsView* aView)
  {
    YTEUnusedArgument(aView);
    return nullptr;
  }

  std::unique_ptr<InstantiatedInfluenceMap> Renderer::CreateWaterInfluenceMap(GraphicsView* aView)
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

  void Renderer::RegisterView(GraphicsView *aView, YTEDrawerTypes aDrawerType, YTEDrawerTypeCombination aCombination)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aDrawerType);
    YTEUnusedArgument(aCombination);
  }

  void Renderer::SetViewDrawingType(GraphicsView *aView, YTEDrawerTypes aDrawerType, YTEDrawerTypeCombination aCombination)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aDrawerType);
    YTEUnusedArgument(aCombination);
  }

  void Renderer::SetViewCombinationType(GraphicsView *aView, YTEDrawerTypeCombination aCombination)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aCombination);
  }

  void Renderer::DeregisterView(GraphicsView *aView)
  {
    YTEUnusedArgument(aView);
  }

  void Renderer::ViewOrderChanged(GraphicsView *aView, float aNewOrder)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aNewOrder);
  }

  void Renderer::ResetView(GraphicsView *aView)
  {
    YTEUnusedArgument(aView);
  }
}
