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
    : InstantiatedModel{}
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
    : InstantiatedModel{}
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

  void DX12InstantiatedModel::Create()
  {
    //auto allocator = mSurface->GetAllocator(DX12AllocatorTypes::UniformBufferObject);
    //auto &device = mSurface->GetDevice();
    //
    //if (false == mVkMesh->GetInstanced())
    //{
    //  // create UBO Per Model buffer
    //  mUBOModel = device->createBuffer(sizeof(UBOs::Model),
    //                                   vk::BufferUsageFlagBits::eTransferDst |
    //                                   vk::BufferUsageFlagBits::eUniformBuffer,
    //                                   vk::SharingMode::eExclusive,
    //                                   nullptr,
    //                                   vk::MemoryPropertyFlagBits::eDeviceLocal,
    //                                   allocator);
    //  
    //  //// create UBO Per Model buffer
    //  mUBOModelMaterial = device->createBuffer(sizeof(UBOs::Material),
    //                                           vk::BufferUsageFlagBits::eTransferDst |
    //                                           vk::BufferUsageFlagBits::eUniformBuffer,
    //                                           vk::SharingMode::eExclusive,
    //                                           nullptr,
    //                                           vk::MemoryPropertyFlagBits::eDeviceLocal,
    //                                           allocator);
    //}
    //
    //// Create UBO Animation Buffer.
    //mUBOAnimation = device->createBuffer(sizeof(UBOs::Animation),
    //                                     vk::BufferUsageFlagBits::eTransferDst |
    //                                     vk::BufferUsageFlagBits::eUniformBuffer,
    //                                     vk::SharingMode::eExclusive,
    //                                     nullptr,
    //                                     vk::MemoryPropertyFlagBits::eDeviceLocal,
    //                                     allocator);
    //
    //UpdateUBOAnimation(&cAnimation);
    //
    //UBOs::Material modelMaterial{};
    //modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    //modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    //modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    //modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    //modelMaterial.mShininess = 1.0f;
    //
    //bool isEditorObject = false;
    //auto name = mMesh->mName;
    //
    //if (name == "Move_X.fbx"   || name == "Move_Y.fbx"   || name == "Move_Z.fbx" ||
    //    name == "Scale_X.fbx"  || name == "Scale_Y.fbx"  || name == "Scale_Z.fbx" ||
    //    name == "Rotate_X.fbx" || name == "Rotate_Y.fbx" || name == "Rotate_Z.fbx")
    //{
    //  isEditorObject = true;
    //}
    //
    //modelMaterial.mFlags = isEditorObject ? (u32)UBOMaterialFlags::IsGizmo : 0;
    //
    //UpdateUBOMaterial(&modelMaterial);
    //
    //mUBOModelData.mModelMatrix = glm::mat4(1.0f);
    //
    //
    //// create descriptor sets
    //for (auto[submesh, i] : enumerate(mVkMesh->mSubmeshes))
    //{
    //  auto materialUBO = device->createBuffer(sizeof(UBOs::Material),
    //                                          vk::BufferUsageFlagBits::eTransferDst |
    //                                          vk::BufferUsageFlagBits::eUniformBuffer,
    //                                          vk::SharingMode::eExclusive,
    //                                          nullptr,
    //                                          vk::MemoryPropertyFlagBits::eDeviceLocal,
    //                                          allocator);
    //
    //  auto &material = submesh->second->mSubmesh->mUBOMaterial;
    //
    //  mUBOSubmeshMaterials.emplace_back(materialUBO, material);
    //
    //  UpdateUBOSubmeshMaterial(&material, i);
    //}
    //
    //CreateShader();
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


  // TODO (Josh): Change the name to be more representative. (This is really 
  //              just telling the object to register to be updated)
  void DX12InstantiatedModel::UpdateUBOModel()
  {
    if (!mLoadUBOAnimation && 
        !mLoadUBOModel && 
        !mLoadUBOMaterial)
    {
      mSurface->RegisterEvent<&DX12InstantiatedModel::DX12GraphicsDataUpdate>(Events::DX12GraphicsDataUpdate, this);
    }
  }

  void DX12InstantiatedModel::UpdateUBOModel(UBOs::Model &aUBO)
  {
    mUBOModelData = aUBO;

    UpdateUBOModel();

    mLoadUBOModel = true;
  }

  void DX12InstantiatedModel::UpdateUBOAnimation(UBOs::Animation *aUBO)
  {
    mUBOAnimationData = aUBO;

    UpdateUBOModel();

    mLoadUBOAnimation = true;
  }

  void DX12InstantiatedModel::UpdateUBOMaterial(UBOs::Material *aUBO)
  {
    mUBOModelMaterialData = *aUBO;

    UpdateUBOModel();

    mLoadUBOMaterial = true;
  }

  void DX12InstantiatedModel::UpdateUBOSubmeshMaterial(UBOs::Material *aUBO, size_t aIndex)
  {
    UnusedArguments(aUBO);
    UnusedArguments(aIndex);
    //mUBOSubmeshMaterials[aIndex].second = *aUBO;

    UpdateUBOModel();

    mLoadUBOMaterial = true;
  }


  void DX12InstantiatedModel::SetDefaultAnimationOffset()
  {
    mUBOAnimationData = mMesh->mSkeleton.GetDefaultOffsets();

    UpdateUBOModel();

    mLoadUBOAnimation = true;
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

  void DX12InstantiatedModel::DX12GraphicsDataUpdate(YTE::DX12GraphicsDataUpdate *aEvent)
  {
    UnusedArguments(aEvent);
    //mSurface->DeregisterEvent<&DX12InstantiatedModel::DX12GraphicsDataUpdate>(Events::DX12GraphicsDataUpdate, this);
    //
    //auto update = aEvent->mCBO;
    //
    //if (mLoadUBOMaterial)
    //{
    //  // TODO: We're updating all materials here, which is unfortunate. Maybe update at the submesh level?
    //  for (auto material : mUBOSubmeshMaterials)
    //  {
    //    material.first->update<UBOs::Material>(0, material.second, update);
    //  }
    //
    //  mUBOModelMaterial->update<UBOs::Material>(0, mUBOModelMaterialData, update);
    //
    //  mLoadUBOMaterial = false;
    //}
    //
    //if (mLoadUBOAnimation)
    //{
    //  mUBOAnimation->update<UBOs::Animation>(0, *mUBOAnimationData, update);
    //  mLoadUBOAnimation = false;
    //}
    //
    //if (mLoadUBOModel)
    //{
    //  if (mVkMesh->GetInstanced())
    //  {
    //    Instance instance(mUBOModelData);
    //
    //    mVkMesh->mInstanceManager.InstanceBuffer()->update<Instance>(mVkMesh->GetOffset(this), instance, update);
    //  }
    //  else
    //  {
    //    mUBOModel->update<UBOs::Model>(0, mUBOModelData, update);
    //  }
    //
    //  mLoadUBOModel = false;
    //}
  }
}

