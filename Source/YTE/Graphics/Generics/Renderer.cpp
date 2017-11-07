///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/InstantiatedSprite.hpp"
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

  std::unique_ptr<InstantiatedSprite> Renderer::CreateSprite(Window *aWindow,
                                                             std::string &aTextureFile)
  {
    YTEUnusedArgument(aWindow);
    YTEUnusedArgument(aTextureFile);

    return nullptr;
  }

  void Renderer::DestroySprite(Window *aWindow, std::unique_ptr<InstantiatedSprite> aSprite)
  {
    YTEUnusedArgument(aWindow);
    YTEUnusedArgument(aSprite);
  }

  std::unique_ptr<InstantiatedModel> Renderer::CreateModel(Window *aWindow,
                                                           std::string &aMeshFile)
  {
    YTEUnusedArgument(aWindow);
    YTEUnusedArgument(aMeshFile);

    return nullptr;
  }

  std::unique_ptr<InstantiatedModel> Renderer::CreateModel(Window *aWindow, Mesh *aMesh)
  {
    YTEUnusedArgument(aWindow);
    YTEUnusedArgument(aMesh);
    return nullptr;
  }

  Mesh* Renderer::CreateSimpleMesh(Window *aWindow, std::string &aName, std::vector<Submesh> &aSubmeshes)
  {
    YTEUnusedArgument(aWindow);
    YTEUnusedArgument(aName);
    YTEUnusedArgument(aSubmeshes);

    return nullptr;
  }

  void Renderer::UpdateWindowViewBuffer(Window *aWindow, UBOView &aView)
  {
    YTEUnusedArgument(aWindow);
    YTEUnusedArgument(aView);
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

  glm::vec4 Renderer::GetClearColor(Window *aWindow)
  {
    YTEUnusedArgument(aWindow);
    return glm::vec4{};
  }

  void Renderer::SetClearColor(Window *aWindow, const glm::vec4 &aColor)
  {
    YTEUnusedArgument(aWindow);
    YTEUnusedArgument(aColor);
  }
}