///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/InstantiatedLight.hpp"
#include "YTE/Graphics/Generics/InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/Texture.hpp"


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

  Texture* Renderer::CreateTexture(std::string &aFilename, TextureType aType)
  {
    YTEUnusedArgument(aFilename);
    YTEUnusedArgument(aType);

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
    YTEUnusedArgument(aName);
    YTEUnusedArgument(aData);
    YTEUnusedArgument(aLayout);
    YTEUnusedArgument(aWidth);
    YTEUnusedArgument(aHeight);
    YTEUnusedArgument(aMipLevels);
    YTEUnusedArgument(aLayerCount);
    YTEUnusedArgument(aType);

    return nullptr;
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

  void Renderer::RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aDrawerType);
    YTEUnusedArgument(aCombination);
  }

  void Renderer::SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination)
  {
    YTEUnusedArgument(aView);
    YTEUnusedArgument(aDrawerType);
    YTEUnusedArgument(aCombination);
  }

  void Renderer::SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination)
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


  Mesh * Renderer::GetBaseMesh(const std::string & aFilename)
  {
    auto mesh = RequestMesh(aFilename);
    if (mesh)
    {
      return mesh;
    }

    // Check for the job, if there is one, wait for it.
    std::shared_lock<std::shared_mutex> reqLock(mRequestedMeshesMutex);
    auto reqIt = mRequestedMeshes.find(aFilename);
    if (reqIt == mRequestedMeshes.end())
    {
      reqLock.unlock();
      return RequestMesh(aFilename);
    }
    auto jobHandle = reqIt->second;
    reqLock.unlock();

    mJobSystem->WaitThisThread(jobHandle);
    std::unique_lock<std::shared_mutex> baseLock(mBaseMeshesMutex);
    auto baseIt = mBaseMeshes.find(aFilename);
    if (baseIt == mBaseMeshes.end())
    {
      mBaseMeshes[aFilename] = std::move(jobHandle.GetReturn().As<std::unique_ptr<Mesh>>());
      mesh = mBaseMeshes[aFilename].get();
    }
    baseLock.unlock();

    std::unique_lock<std::shared_mutex> reqUniqueLock(mBaseMeshesMutex);
    reqIt = mRequestedMeshes.find(aFilename);
    if (reqIt != mRequestedMeshes.end())
    {
      mRequestedMeshes.erase(reqIt);
    }
    reqUniqueLock.unlock();

    if (mesh)
    {
      return mesh;
    }

    // taking advantage of the fact that this does locking of the base mesh
    return RequestMesh(aFilename);
  }

  Texture* Renderer::GetBaseTexture(const std::string &aFilename)
  {
    auto texture = RequestTexture(aFilename);
    if (texture)
    {
      return texture;
    }

    // Check for the job, if there is one, wait for it.
    std::shared_lock<std::shared_mutex> reqLock(mRequestedTexturesMutex);
    auto reqIt = mRequestedTextures.find(aFilename);
    if (reqIt == mRequestedTextures.end())
    {
      reqLock.unlock();
      return RequestTexture(aFilename);
    }
    auto jobHandle = reqIt->second;
    reqLock.unlock();

    mJobSystem->WaitThisThread(jobHandle);
    std::unique_lock<std::shared_mutex> baseLock(mBaseTexturesMutex);
    auto baseIt = mBaseTextures.find(aFilename);
    if (baseIt == mBaseTextures.end())
    {
      mBaseTextures[aFilename] = std::move(jobHandle.GetReturn().As<std::unique_ptr<Texture>>());
      texture = mBaseTextures[aFilename].get();
    }
    baseLock.unlock();

    std::unique_lock<std::shared_mutex> reqUniqueLock(mBaseTexturesMutex);
    reqIt = mRequestedTextures.find(aFilename);
    if (reqIt != mRequestedTextures.end())
    {
      mRequestedTextures.erase(reqIt);
    }
    reqUniqueLock.unlock();

    if (texture)
    {
      return texture;
    }

    // taking advantage of the fact that this does locking of the base texture
    return RequestTexture(aFilename);
  }

  Mesh* Renderer::RequestMesh(const std::string &aMeshFile)
  {
    std::shared_lock<std::shared_mutex> baseLock(mBaseMeshesMutex);
    auto baseIt = mBaseMeshes.find(aMeshFile);
    // if already loaded back out
    if (baseIt != mBaseMeshes.end())
    {
      return baseIt->second.get();
    }
    baseLock.unlock();

    std::unique_lock<std::shared_mutex> reqLock(mRequestedMeshesMutex);
    auto reqIt = mRequestedMeshes.find(aMeshFile);
    // if already loading back out
    if (reqIt != mRequestedMeshes.end())
    {
      return nullptr;
    }

    // Not in the futures map, add it.
    mRequestedMeshes[aMeshFile] = mJobSystem->QueueJobThisThread([aMeshFile](JobHandle& handle)->Any {
      auto mesh = std::make_unique<Mesh>(aMeshFile);
      return Any{ mesh };
    });
    return nullptr;
  }

  Texture* Renderer::RequestTexture(const std::string &aFilename)
  {
    std::shared_lock<std::shared_mutex> baseLock(mBaseTexturesMutex);
    auto baseIt = mBaseTextures.find(aFilename);
    // if already loaded back out
    if (baseIt != mBaseTextures.end())
    {
      return baseIt->second.get();
    }
    baseLock.unlock();

    std::unique_lock<std::shared_mutex> reqLock(mRequestedTexturesMutex);
    auto reqIt = mRequestedTextures.find(aFilename);
    // if already loading back out
    if (reqIt != mRequestedTextures.end())
    {
      return nullptr;
    }

    // Not in the futures map, add it.
    mRequestedMeshes[aFilename] = mJobSystem->QueueJobThisThread([aFilename](JobHandle& handle)->Any {
      auto texture = std::make_unique<Texture>(aFilename);
      return Any{ texture };
    });
    return nullptr;
  }
}
