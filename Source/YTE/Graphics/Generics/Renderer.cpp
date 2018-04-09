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


  Texture* Renderer::GetBaseTexture(std::string &aFilename)
  {
    RequestTexture(aFilename);

    // Check for the job, if there is one, wait for it.
    {

    }

    // We've waited for the job, the asset should be there.
    {
      std::shared_lock<std::shared_mutex> lockBaseMesh(mBaseMeshMutex);

      auto it = mBaseTextures.find(aFilename);

      if (it != mBaseTextures.end())
      {
        return it->second.get();
      }
    }

    return nullptr;
  }

  void Renderer::RequestMesh(std::string &aMeshFile)
  {
    bool tryToAdd = false;

    {
      std::shared_lock<std::shared_mutex> lockBaseMesh(mBaseMeshMutex);

      auto it = mBaseMeshes.find(aMeshFile);

      if (it == mBaseMeshes.end())
      {
        // Not in the finished map, check the futures
        std::shared_lock<std::shared_mutex> lockBaseMeshThread(mBaseMeshThreadMutex);

        auto it2 = mMeshThreadDatas.find(aMeshFile);

        if (it2 == mMeshThreadDatas.end())
        {
          // Not in the futures map, try to add it.
          tryToAdd = true;
        }
      }
    }

    if (tryToAdd)
    {
      std::unique_lock<std::shared_mutex> lockBaseMeshThread(mBaseMeshThreadMutex);

      auto it2 = mMeshThreadDatas.find(aMeshFile);

      if (it2 == mMeshThreadDatas.end())
      {
        // Not in the futures map, add it.
        auto &meshFuture = mMeshThreadDatas[aMeshFile];

        auto delegate = YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>,
                                        &(meshFuture),
                                        &MeshThreadData::MakeMesh);
        meshFuture.mName = aMeshFile;
        meshFuture.mRenderer = this;
        meshFuture.mHandle = mJobSystem->QueueJobThisThread(std::move(delegate));
      }
    }
  }

  void Renderer::RequestTexture(std::string &aFilename)
  {
    bool tryToAdd = false;

    {
      std::shared_lock<std::shared_mutex> lockBaseTexture(mBaseTextureMutex);

      auto it = mBaseTextures.find(aFilename);

      if (it == mBaseTextures.end())
      {
        // Not in the finished map, check the futures
        std::shared_lock<std::shared_mutex> lockBaseTextureThread(mBaseTextureThreadMutex);

        auto it2 = mTextureThreadDatas.find(aFilename);

        if (it2 == mTextureThreadDatas.end())
        {
          // Not in the futures map, try to add it.
          tryToAdd = true;
        }
      }
    }

    if (tryToAdd)
    {
      std::unique_lock<std::shared_mutex> lockBaseTextureThread(mBaseTextureThreadMutex);

      auto it2 = mTextureThreadDatas.find(aFilename);

      if (it2 == mTextureThreadDatas.end())
      {
        // Not in the futures map, add it.
        auto &textureFuture = mTextureThreadDatas[aFilename];

        auto delegate = YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>,
                                        &(textureFuture),
                                        &TextureThreadData::MakeTexture);

        textureFuture.mName = aFilename;
        textureFuture.mRenderer = this;
        textureFuture.mHandle = mJobSystem->QueueJobThisThread(std::move(delegate));
      }
    }
  }

  Any Renderer::MeshThreadData::MakeMesh(JobHandle&)
  {
    auto mesh = std::make_unique<Mesh>(mName);

    std::unique_lock<std::shared_mutex> lockBaseMesh(mRenderer->mBaseMeshMutex);
    mRenderer->mBaseMeshes[mName] = std::move(mesh);

    std::unique_lock<std::shared_mutex> lockBaseMeshThreadData(mRenderer->mBaseMeshThreadMutex);
    mRenderer->mMeshThreadDatas.erase(mRenderer->mMeshThreadDatas.find(mName));

    return Any{};
  }

  Renderer::MeshThreadData::~MeshThreadData()
  {

  }
  
  Any Renderer::TextureThreadData::MakeTexture(JobHandle&)
  {
    auto texture = std::make_unique<Texture>(mName);

    std::unique_lock<std::shared_mutex> lockBaseTexture(mRenderer->mBaseTextureMutex);
    mRenderer->mBaseTextures[mName] = std::move(texture);

    std::unique_lock<std::shared_mutex> lockBaseTextureThreadData(mRenderer->mBaseTextureThreadMutex);
    mRenderer->mTextureThreadDatas.erase(mRenderer->mTextureThreadDatas.find(mName));

    return Any{};
  }

  Renderer::TextureThreadData::~TextureThreadData()
  {

  }
}
