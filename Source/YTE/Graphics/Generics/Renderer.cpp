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

  Texture* Renderer::GetTexture(std::string &aFilename)
  {
    YTEUnusedArgument(aFilename);
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


  Texture* Renderer::GetBaseTexture(std::string &aFilename)
  {
    mBaseMeshFutureMutex.lock_shared();
    auto it = mBaseTextures.find(aFilename);

    if (it != mBaseTextures.end())
    {
    }
    else
    {

    }
  }

  void Renderer::RequestMesh(std::string &aMeshFile)
  {
    std::shared_lock<std::shared_mutex> lock(mBaseMeshMutex);

    auto it = mBaseMeshes.find(aMeshFile);

    if (it != mBaseMeshes.end())
    {
      // Not in the finished map, check the futures
      mBaseMeshFutureMutex.lock_shared();

      auto it2 = mMeshFutures.find(aMeshFile);

      if (it2 != mMeshFutures.end())
      {
        // Not in the futures map, add it.
        mBaseMeshFutureMutex.unlock_shared();
        mBaseMeshFutureMutex.lock();
        auto &meshFuture = mMeshFutures[aMeshFile];
        mBaseMeshFutureMutex.unlock();
        mBaseMeshFutureMutex.lock_shared();

        meshFuture.first.mName = aMeshFile;

        meshFuture.first.mHandle = mJobSystem->QueueJobThisThread(YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>,
                                                                                  &(meshFuture.first),
                                                                                  &MeshThreadData::MakeMesh));

        meshFuture.second = meshFuture.first.mPromise.get_future();
      }

      mBaseMeshFutureMutex.unlock_shared();
    }
  }

  void Renderer::RequestTexture(std::string &aFilename)
  {
    std::shared_lock<std::shared_mutex> lock(mBaseTextureMutex);

    auto it = mBaseTextures.find(aFilename);

    if (it != mBaseTextures.end())
    {
      // Not in the finished map, check the futures
      mBaseTextureFutureMutex.lock_shared();

      auto it2 = mTextureFutures.find(aFilename);

      if (it2 != mTextureFutures.end())
      {
        // Not in the futures map, add it.
        mBaseTextureFutureMutex.unlock_shared();
        mBaseTextureFutureMutex.lock();
        auto &textureFuture = mTextureFutures[aFilename];
        mBaseTextureFutureMutex.unlock();
        mBaseTextureFutureMutex.lock_shared();

        textureFuture.first.mName = aFilename;

        textureFuture.first.mHandle = mJobSystem->QueueJobThisThread(YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>,
                                                                                     &(textureFuture.first),
                                                                                     &TextureThreadData::MakeTexture));

        textureFuture.second = textureFuture.first.mPromise.get_future();
      }

      mBaseTextureFutureMutex.unlock_shared();
    }
  }

  Any Renderer::MeshThreadData::MakeMesh(JobHandle&)
  {
    mPromise.set_value(std::make_unique<Mesh>(mName));
  }

  Renderer::MeshThreadData::~MeshThreadData()
  {

  }
  
  Any Renderer::TextureThreadData::MakeTexture(JobHandle&)
  {
    mPromise.set_value(std::make_unique<Texture>(mName));
  }

  Renderer::TextureThreadData::~TextureThreadData()
  {

  }
}
