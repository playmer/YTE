///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedModel)
  {
    RegisterType<VkInstantiatedModel>();
    TypeBuilder<VkInstantiatedModel> builder;
  }

  UBOs::Animation VkInstantiatedModel::cAnimation;

  VkInstantiatedModel::VkInstantiatedModel(std::string &aModelFile,
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{}
    , mSurface{aSurface}
    , mView{ aView }
  {
    mVkMesh = mSurface->GetRenderer()->CreateMesh(aModelFile);
    mMesh = mVkMesh->mMesh;
    Create();

    mView->RegisterEvent<&VkInstantiatedModel::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&VkInstantiatedModel::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  VkInstantiatedModel::VkInstantiatedModel(VkMesh *aMesh, 
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{}
    , mSurface{ aSurface }
    , mView{ aView }
    , mVkMesh{aMesh}
  {
    mMesh = aMesh->mMesh;
    Create();

    mView->RegisterEvent<&VkInstantiatedModel::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&VkInstantiatedModel::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  VkInstantiatedModel::~VkInstantiatedModel()
  {
    mSurface->DestroyModel(mView, this);
  }

  void VkInstantiatedModel::SurfaceLostEvent(ViewChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mSurface->DestroyModel(mView, this);
  }

  void VkInstantiatedModel::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    mView = aEvent->View;
    mSurface = static_cast<VkRenderer*>(mView->GetRenderer())->GetSurface(mView->GetWindow());
    CreateShader();
    mSurface->AddModel(this);
  }

  void VkInstantiatedModel::Create()
  {
    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);
    auto &device = mSurface->GetDevice();

    // create UBO Per Model buffer
    mUBOModel = device->createBuffer(sizeof(UBOs::Model),
                                      vk::BufferUsageFlagBits::eTransferDst |
                                      vk::BufferUsageFlagBits::eUniformBuffer,
                                      vk::SharingMode::eExclusive,
                                      nullptr,
                                      vk::MemoryPropertyFlagBits::eDeviceLocal,
                                      allocator);
      
    // create UBO Per Model Material buffer
    mUBOModelMaterial = device->createBuffer(sizeof(UBOs::Material),
                                             vk::BufferUsageFlagBits::eTransferDst |
                                             vk::BufferUsageFlagBits::eUniformBuffer,
                                             vk::SharingMode::eExclusive,
                                             nullptr,
                                             vk::MemoryPropertyFlagBits::eDeviceLocal,
                                             allocator);

    // Create UBO Animation Buffer.
    mUBOAnimation = device->createBuffer(sizeof(UBOs::Animation),
                                         vk::BufferUsageFlagBits::eTransferDst |
                                         vk::BufferUsageFlagBits::eUniformBuffer,
                                         vk::SharingMode::eExclusive,
                                         nullptr,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         allocator);

    UpdateUBOAnimation(&cAnimation);

    UBOs::Material modelMaterial{};
    modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mShininess = 1.0f;

    bool isEditorObject = false;
    auto name = mMesh->mName;

    if (name == "Move_X.fbx"   || name == "Move_Y.fbx"   || name == "Move_Z.fbx" ||
        name == "Scale_X.fbx"  || name == "Scale_Y.fbx"  || name == "Scale_Z.fbx" ||
        name == "Rotate_X.fbx" || name == "Rotate_Y.fbx" || name == "Rotate_Z.fbx")
    {
      isEditorObject = true;
    }

    modelMaterial.mFlags = isEditorObject ? (u32)UBOs::MaterialFlags::IsGizmo : 0;

    UpdateUBOMaterial(&modelMaterial);

    mUBOModelData.mModelMatrix = glm::mat4(1.0f);


    // create descriptor sets
    for (auto[submeshIt, i] : enumerate(mVkMesh->mSubmeshes))
    {
      auto& submesh = *submeshIt;

      auto materialUBO = device->createBuffer(sizeof(UBOs::Material),
                                              vk::BufferUsageFlagBits::eTransferDst |
                                              vk::BufferUsageFlagBits::eUniformBuffer,
                                              vk::SharingMode::eExclusive,
                                              nullptr,
                                              vk::MemoryPropertyFlagBits::eDeviceLocal,
                                              allocator);

      auto &material = submesh->mSubmesh->mUBOMaterial;

      mUBOSubmeshMaterials.emplace_back(materialUBO, material);

      UpdateUBOSubmeshMaterial(&material, i);
    }

    CreateShader();
  }

  void VkInstantiatedModel::CreateShader()
  {
    mPipelineData.clear();

    // create descriptor sets
    for (auto[submeshIt, i] : enumerate(mVkMesh->mSubmeshes))
    {
      auto& submesh = *submeshIt;

      submesh->CreateShader(mView);

      CreateDescriptorSet(submesh.get(), i);
    }
  }

  void VkInstantiatedModel::UpdateMesh(size_t aIndex, 
                                       std::vector<Vertex>& aVertices)
  {
    mVkMesh->UpdateVertices(aIndex, aVertices);
  }

  void VkInstantiatedModel::UpdateMesh(size_t aIndex, 
                                       std::vector<Vertex>& aVertices,
                                       std::vector<u32>& aIndices)
  {
    mVkMesh->UpdateVerticesAndIndices(aIndex, aVertices, aIndices);
  }

  void VkInstantiatedModel::UpdateUBOModel(UBOs::Model &aUBO)
  {
    mUBOModelData = aUBO;

    mSurface->GetRenderer()->mUBOUpdates.Add(mUBOModel, mUBOModelData);
  }

  void VkInstantiatedModel::UpdateUBOAnimation(UBOs::Animation *aUBO)
  {
    mUBOAnimationData = aUBO;

    mSurface->GetRenderer()->mUBOUpdates.Add(mUBOAnimation, *mUBOAnimationData);
  }

  void VkInstantiatedModel::UpdateUBOMaterial(UBOs::Material *aUBO)
  {
    mUBOModelMaterialData = *aUBO;
    mSurface->GetRenderer()->mUBOUpdates.Add(mUBOModelMaterial, mUBOModelMaterialData);
  }

  void VkInstantiatedModel::UpdateUBOSubmeshMaterial(UBOs::Material *aUBO, size_t aIndex)
  {
    auto& [buffer, ubo] = mUBOSubmeshMaterials[aIndex];
    ubo = *aUBO;

    mSurface->GetRenderer()->mUBOUpdates.Add(buffer, ubo);
  }

  void VkInstantiatedModel::SetDefaultAnimationOffset()
  {
    UpdateUBOAnimation(mMesh->mSkeleton.GetDefaultOffsets());
  }

  void VkInstantiatedModel::CreateDescriptorSet(VkSubmesh *aSubMesh, size_t aIndex)
  {
    mPipelineData.emplace(aSubMesh, 
                          aSubMesh->CreatePipelineData(mUBOModel, 
                                                       mUBOAnimation,
                                                       mUBOModelMaterial,
                                                       mUBOSubmeshMaterials[aIndex].first,
                                                       mView));
  }
}

