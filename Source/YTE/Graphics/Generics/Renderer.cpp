#include "YTE/Core/Engine.hpp"

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
    RegisterType<Renderer>();
    TypeBuilder<Renderer> builder;
  }


  Renderer::Renderer(Engine *aEngine)  
    : mJobSystem{ aEngine->GetComponent<JobSystem>() }
  {
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
  
  Mesh* Renderer::CreateSimpleMesh(std::string &aName, ContiguousRange<SubmeshData> aSubmeshes, bool aForceUpdate)
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



  void Renderer::UpdateWindowViewBuffer(GraphicsView *aView, UBOs::View &aUBOView)
  {
    UnusedArguments(aView, aUBOView);
  }

  void Renderer::UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOs::Illumination &aIllumination)
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


  Mesh * Renderer::GetBaseMesh(const std::string & aFilename)
  {
    YTEProfileFunction();
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
      mBaseMeshes[aFilename] = std::unique_ptr<Mesh>(jobHandle.GetReturn().As<Mesh*>());
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
    YTEProfileFunction();
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
      mBaseTextures[aFilename] = std::unique_ptr<Texture>(jobHandle.GetReturn().As<Texture*>());
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
    YTEProfileFunction();
    //((Engine*)mJobSystem->GetOwner())->Log(LogType::Information, fmt::format("Requesting mesh: {}", aMeshFile));
    
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
    mRequestedMeshes[aMeshFile] = mJobSystem->QueueJobThisThread([this ,aMeshFile](JobHandle& handle)->Any {
      UnusedArguments(handle);
      auto mesh = new Mesh(this, aMeshFile);
      return Any{ mesh };
    });
    return nullptr;
  }

  Texture* Renderer::RequestTexture(const std::string &aFilename)
  {
    YTEProfileFunction();
    //((Engine*)mJobSystem->GetOwner())->Log(LogType::Information, fmt::format("Requesting texture: {}", aFilename));
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
    mRequestedTextures[aFilename] = mJobSystem->QueueJobThisThread([aFilename](JobHandle& handle)->Any {
      UnusedArguments(handle);
      auto texture = new Texture(aFilename);
      return Any{ texture };
    });
    return nullptr;
  }
}
