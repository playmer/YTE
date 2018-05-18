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
    RegisterType<Renderer>();
    TypeBuilder<Renderer> builder;
  }


  Renderer::~Renderer()
  {

  }

  void Renderer::RegisterWindowForDraw(Window *aWindow)
  {
    UnusedArguments(aWindow);
  }

  void Renderer::DeregisterWindowFromDraw(Window * aWindow)
  {
    UnusedArguments(aWindow);
  }

  std::unique_ptr<InstantiatedModel> Renderer::CreateModel(GraphicsView *aView,
                                                           std::string &aMeshFile)
  {
    UnusedArguments(aView, aMeshFile);

    return nullptr;
  }

  std::unique_ptr<InstantiatedModel> Renderer::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    UnusedArguments(aView, aMesh);
    return nullptr;
  }

  void Renderer::DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel)
  {
    UnusedArguments(aView, aModel);
  }

  Texture* Renderer::CreateTexture(std::string &aFilename, TextureType aType)
  {
    UnusedArguments(aFilename, aType);

    return nullptr;
  }

  Texture* Renderer::CreateTexture(std::string aName,
                                   std::vector<u8> aData,
                                   TextureLayout aLayout,
                                   u32 aWidth,
                                   u32 aHeight,
                                   u32 aMipLevels,
                                   u32 aLayerCount,
                                   TextureType aType)
  {
    UnusedArguments(aName, aData, aLayout, aWidth, aHeight, aMipLevels, aLayerCount, aType);

    return nullptr;
  }

  Texture* Renderer::GetTexture(std::string &aFilename)
  {
    UnusedArguments(aFilename);
    return nullptr;
  }

  Mesh* Renderer::CreateSimpleMesh(std::string &aName, std::vector<Submesh> &aSubmeshes, bool aForceUpdate)
  {
    UnusedArguments(aName, aSubmeshes, aForceUpdate);

    return nullptr;
  }

  std::unique_ptr<InstantiatedLight> Renderer::CreateLight(GraphicsView* aView)
  {
    UnusedArguments(aView);
    return nullptr;
  }

  std::unique_ptr<InstantiatedInfluenceMap> Renderer::CreateWaterInfluenceMap(GraphicsView* aView)
  {
    UnusedArguments(aView);
    return nullptr;
  }



  void Renderer::UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView)
  {
    UnusedArguments(aView, aUBOView);
  }

  void Renderer::UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOIllumination &aIllumination)
  {
    UnusedArguments(aView, aIllumination);
  }


  void Renderer::GraphicsDataUpdate(LogicUpdate *aEvent)
  {
    UnusedArguments(aEvent);
  }

  void Renderer::FrameUpdate(LogicUpdate *aEvent)
  {
    UnusedArguments(aEvent);
  }

  void Renderer::PresentFrame(LogicUpdate *aEvent)
  {
    UnusedArguments(aEvent);
  }

  glm::vec4 Renderer::GetClearColor(GraphicsView *aView)
  {
    UnusedArguments(aView);
    return glm::vec4{};
  }

  void Renderer::SetClearColor(GraphicsView *aView, const glm::vec4 &aColor)
  {
    UnusedArguments(aView, aColor);
  }

  void Renderer::AnimationUpdate(LogicUpdate* aEvent)
  {
    UnusedArguments(aEvent);
  }

  void Renderer::SetLights(bool aOnOrOff)
  {
    UnusedArguments(aOnOrOff);
  }

  void Renderer::RegisterView(GraphicsView *aView)
  {
    UnusedArguments(aView);
  }

  void Renderer::RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    UnusedArguments(aView, aDrawerType, aCombination);
  }

  void Renderer::SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    UnusedArguments(aView, aDrawerType, aCombination);
  }

  void Renderer::SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination)
  {
    UnusedArguments(aView, aCombination);
  }

  void Renderer::DeregisterView(GraphicsView *aView)
  {
    UnusedArguments(aView);
  }

  void Renderer::ViewOrderChanged(GraphicsView *aView, float aNewOrder)
  {
    UnusedArguments(aView, aNewOrder);
  }

  void Renderer::ResetView(GraphicsView *aView)
  {
    UnusedArguments(aView);
  }
}
