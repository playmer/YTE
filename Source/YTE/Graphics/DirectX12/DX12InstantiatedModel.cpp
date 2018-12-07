#include "YTE/Graphics/DirectX12/DX12InstantiatedModel.hpp"
#include "YTE/Graphics/DirectX12/DX12Mesh.hpp"
#include "YTE/Graphics/DirectX12/DX12Renderer.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Texture.hpp"
#include "YTE/Graphics/DirectX12/DX12DeviceInfo.hpp"

namespace YTE
{
  YTEDefineType(DX12InstantiatedModel)
  {
    RegisterType<DX12InstantiatedModel>();
    TypeBuilder<DX12InstantiatedModel> builder;
  }

  UBOs::Animation DX12InstantiatedModel::cAnimation;

  DX12InstantiatedModel::DX12InstantiatedModel(std::string &aModelFile,
                                           Dx12RenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{ aSurface->GetRenderer() }
    , mSurface{aSurface}
    , mView{ aView }
    , mLoadUBOAnimation{ false }
    , mLoadUBOModel{ false }
    , mLoadUBOMaterial{ false }
  {
    mVkMesh = mSurface->GetRenderer()->CreateMesh(aModelFile);
    mMesh = mVkMesh->mMesh;
    Create();

    mView->RegisterEvent<&DX12InstantiatedModel::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&DX12InstantiatedModel::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  DX12InstantiatedModel::DX12InstantiatedModel(DX12Mesh *aMesh, 
                                           Dx12RenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{ aSurface->GetRenderer() }
    , mSurface{ aSurface }
    , mView{ aView }
    , mLoadUBOAnimation{false}
    , mLoadUBOModel{ false }
    , mLoadUBOMaterial{ false }
    , mVkMesh{aMesh}
  {
    mMesh = aMesh->mMesh;
    Create();

    mView->RegisterEvent<&DX12InstantiatedModel::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&DX12InstantiatedModel::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  DX12InstantiatedModel::~DX12InstantiatedModel()
  {
    mSurface->DestroyModel(mView, this);
  }

  void DX12InstantiatedModel::SurfaceLostEvent(ViewChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mSurface->DestroyModel(mView, this);
  }

  void DX12InstantiatedModel::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    mView = aEvent->View;
    mSurface = static_cast<Dx12Renderer*>(mView->GetRenderer())->GetSurface(mView->GetWindow());
    CreateShader();
    mSurface->AddModel(this);
  }

  void DX12InstantiatedModel::CreateShader()
  {
    mPipelineData.clear();

    // create descriptor sets
    for (auto[submesh, i] : enumerate(mVkMesh->mSubmeshes))
    {
      submesh->second->CreateShader(mView);

      CreateDescriptorSet(submesh->second.get(), i);
    }
  }

  void DX12InstantiatedModel::UpdateMesh(size_t aIndex, 
                                       std::vector<Vertex>& aVertices)
  {
    mVkMesh->UpdateVertices(aIndex, aVertices);
  }

  void DX12InstantiatedModel::UpdateMesh(size_t aIndex, 
                                       std::vector<Vertex>& aVertices,
                                       std::vector<u32>& aIndices)
  {
    mVkMesh->UpdateVerticesAndIndices(aIndex, aVertices, aIndices);
  }

  void DX12InstantiatedModel::CreateDescriptorSet(Dx12Submesh *aSubMesh, size_t aIndex)
  {
    UnusedArguments(aSubMesh);
    UnusedArguments(aIndex);
    //mPipelineData.emplace(aSubMesh, 
    //                      aSubMesh->CreatePipelineData(mUBOModel, 
    //                                                   mUBOAnimation,
    //                                                   mUBOModelMaterial,
    //                                                   mUBOSubmeshMaterials[aIndex].first,
    //                                                   mView));
  }
}

