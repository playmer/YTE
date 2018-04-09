///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_Renderer_hpp
#define YTE_Graphics_Generics_Renderer_hpp

#include <future>
#include <shared_mutex>

#include "YTE/Core/Threading/JobSystem.hpp"
#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/Texture.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

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
    virtual void DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel);

    virtual Texture* CreateTexture(std::string &aFilename, TextureType aType);
    virtual Texture* CreateTexture(std::string aName,
                                   std::vector<u8> aData,
                                   TextureLayout aLayout,
                                   u32 aWidth,
                                   u32 aHeight,
                                   u32 aMipLevels,
                                   u32 aLayerCount,
                                   TextureType aType);

    virtual Mesh* CreateSimpleMesh(std::string &aName,
                                   std::vector<Submesh> &aSubmeshes,
			                             bool aForceUpdate = false);
    virtual std::unique_ptr<InstantiatedLight> CreateLight(GraphicsView *aView);
    virtual std::unique_ptr<InstantiatedInfluenceMap> CreateWaterInfluenceMap(GraphicsView *aView);

    virtual void UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView);
    virtual void UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOIllumination &aIllumination);
    virtual void GraphicsDataUpdate(LogicUpdate *aEvent);
    virtual void FrameUpdate(LogicUpdate *aEvent);
    virtual void PresentFrame(LogicUpdate *aEvent);
    virtual glm::vec4 GetClearColor(GraphicsView *aView);
    virtual void SetClearColor(GraphicsView *aView, const glm::vec4 &aColor);
    virtual void AnimationUpdate(LogicUpdate *aEvent);

    virtual void SetLights(bool aOnOrOff);  // true for on, false for off
    virtual void RegisterView(GraphicsView *aView);
    virtual void RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    virtual void SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    virtual void SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination);
    virtual void DeregisterView(GraphicsView *aView);
    virtual void ViewOrderChanged(GraphicsView *aView, float aNewOrder);

    virtual void ResetView(GraphicsView *aView);

    void RequestMesh(std::string &aMeshFile);
    void RequestTexture(std::string &aFilename);

    struct MeshThreadData
    {
      MeshThreadData();
      ~MeshThreadData();

      Any MakeMesh(JobHandle&);

      Renderer *mRenderer;
      std::string mName;
      JobHandle mHandle;
    };

    struct TextureThreadData
    {
      ~TextureThreadData();

      Any MakeTexture(JobHandle&);

      Renderer *mRenderer;
      std::string mName;
      JobHandle mHandle;
    };

    Mesh* GetBaseMesh(std::string &aFilename);
    Texture* GetBaseTexture(std::string &aFilename);

  private:
    std::unordered_map<std::string, MeshThreadData> mMeshThreadDatas;
    std::shared_mutex mBaseMeshThreadMutex;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> mBaseMeshes;
    std::shared_mutex mBaseMeshMutex;

    std::unordered_map<std::string, TextureThreadData> mTextureThreadDatas;
    std::shared_mutex mBaseTextureThreadMutex;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mBaseTextures;
    std::shared_mutex mBaseTextureMutex;

    JobSystem *mJobSystem;
  };
}

#endif
