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
    YTERegisterType(VkInstantiatedModel);
  }

  VkInstantiatedModel::VkInstantiatedModel(std::string &aModelFile,
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel()
    , mSurface(aSurface)
    , mView(aView)
    , mLoadUBOAnimation(false)
    , mLoadUBOModel(false)
    , mLoadUBOMaterial(false)
  {
    mMesh = mSurface->GetRenderer()->CreateMesh(aModelFile);
    Create();

    mView->YTERegister(Events::SurfaceLost, this, &VkInstantiatedModel::SurfaceLostEvent);
    mView->YTERegister(Events::SurfaceGained, this, &VkInstantiatedModel::SurfaceGainedEvent);
  }

  VkInstantiatedModel::VkInstantiatedModel(Mesh *aMesh, 
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel()
    , mSurface(aSurface)
    , mView(aView)
    , mLoadUBOAnimation(false)
    , mLoadUBOModel(false)
    , mLoadUBOMaterial(false)
  {
    mMesh = static_cast<VkMesh*>(aMesh);
    Create();

    mView->YTERegister(Events::SurfaceLost, this, &VkInstantiatedModel::SurfaceLostEvent);
    mView->YTERegister(Events::SurfaceGained, this, &VkInstantiatedModel::SurfaceGainedEvent);
  }

  VkInstantiatedModel::~VkInstantiatedModel()
  {
    mSurface->DestroyModel(mView, this);
  }

  void VkInstantiatedModel::SurfaceLostEvent(ViewChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);
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
    auto mesh = static_cast<VkMesh*>(mMesh);

    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);
    auto &device = mSurface->GetDevice();

    if (false == mesh->GetInstanced())
    {
      // create UBO Per Model buffer
      mUBOModel = device->createBuffer(sizeof(UBOModel),
                                       vk::BufferUsageFlagBits::eTransferDst |
                                       vk::BufferUsageFlagBits::eUniformBuffer,
                                       vk::SharingMode::eExclusive,
                                       nullptr,
                                       vk::MemoryPropertyFlagBits::eDeviceLocal,
                                       allocator);
      
      //// create UBO Per Model buffer
      mUBOModelMaterial = device->createBuffer(sizeof(UBOMaterial),
                                               vk::BufferUsageFlagBits::eTransferDst |
                                               vk::BufferUsageFlagBits::eUniformBuffer,
                                               vk::SharingMode::eExclusive,
                                               nullptr,
                                               vk::MemoryPropertyFlagBits::eDeviceLocal,
                                               allocator);
    }

    // Create UBO Animation Buffer.
    mUBOAnimation = device->createBuffer(sizeof(UBOAnimation),
                                         vk::BufferUsageFlagBits::eTransferDst |
                                         vk::BufferUsageFlagBits::eUniformBuffer,
                                         vk::SharingMode::eExclusive,
                                         nullptr,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         allocator);



    UBOMaterial modelMaterial{};
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

    modelMaterial.mFlags = isEditorObject ? (u32)UBOMaterialFlags::IsGizmo : 0;

    UpdateUBOMaterial(&modelMaterial);

    mUBOModelData.mModelMatrix = glm::mat4(1.0f);


    // create descriptor sets
    for (auto[submesh, i] : enumerate(mesh->mSubmeshes))
    {
      auto materialUBO = device->createBuffer(sizeof(UBOMaterial),
                                              vk::BufferUsageFlagBits::eTransferDst |
                                              vk::BufferUsageFlagBits::eUniformBuffer,
                                              vk::SharingMode::eExclusive,
                                              nullptr,
                                              vk::MemoryPropertyFlagBits::eDeviceLocal,
                                              allocator);

      auto &material = submesh->second->mSubmesh->mUBOMaterial;

      mUBOSubmeshMaterials.emplace_back(materialUBO, material);

      UpdateUBOSubmeshMaterial(&material, i);
    }

    CreateShader();
  }

  void VkInstantiatedModel::CreateShader()
  {
    auto mesh = static_cast<VkMesh*>(mMesh);
    mPipelineData.clear();

    // create descriptor sets
    for (auto[submesh, i] : enumerate(mesh->mSubmeshes))
    {
      submesh->second->CreateShader(mView);

      CreateDescriptorSet(submesh->second.get(), i);
    }
  }


  // TODO (Josh): Change the name to be more representative. (This is really 
  //              just telling the object to register to be updated)
  void VkInstantiatedModel::UpdateUBOModel()
  {
    if (!mLoadUBOAnimation && 
        !mLoadUBOModel && 
        !mLoadUBOMaterial)
    {
      mSurface->YTERegister(Events::GraphicsDataUpdateVk,
                            this,
                            &VkInstantiatedModel::GraphicsDataUpdateVk);
    }
  }

  void VkInstantiatedModel::UpdateUBOModel(UBOModel &aUBO)
  {
    mUBOModelData = aUBO;

    UpdateUBOModel();

    mLoadUBOModel = true;
  }

  void VkInstantiatedModel::UpdateUBOAnimation(UBOAnimation *aUBO)
  {
    mUBOAnimationData = aUBO;

    UpdateUBOModel();

    mLoadUBOAnimation = true;
  }

  void VkInstantiatedModel::UpdateUBOMaterial(UBOMaterial *aUBO)
  {
    mUBOModelMaterialData = *aUBO;

    UpdateUBOModel();

    mLoadUBOMaterial = true;
  }

  void VkInstantiatedModel::UpdateUBOSubmeshMaterial(UBOMaterial *aUBO, size_t aIndex)
  {
    mUBOSubmeshMaterials[aIndex].second = *aUBO;

    UpdateUBOModel();

    mLoadUBOMaterial = true;
  }


  void VkInstantiatedModel::SetDefaultAnimationOffset()
  {
    mUBOAnimationData = mMesh->mSkeleton.GetDefaultOffsets();

    UpdateUBOModel();

    mLoadUBOAnimation = true;
  }

  void VkInstantiatedModel::CreateDescriptorSet(VkSubmesh *aSubMesh, size_t aIndex)
  {
    std::vector<std::shared_ptr<vkhlf::Buffer>> buffers;

    auto& ubos = aSubMesh->mSubmesh->mUBOs;
    for (int i = 0; i < ubos.size(); ++i)
    {
      if (UBOTypeIDs::View == ubos[i].mTypeID)
      {
        buffers.push_back(mSurface->GetUBOViewBuffer(mView)); // View
      }
      else if (UBOTypeIDs::Animation == ubos[i].mTypeID)
      {
        buffers.push_back(mUBOAnimation); // Animation
      }
      else if (UBOTypeIDs::ModelMaterial == ubos[i].mTypeID)
      {
        buffers.push_back(mUBOModelMaterial); // Model Material
      }
      else if (UBOTypeIDs::SubmeshMaterial == ubos[i].mTypeID)
      {
        buffers.push_back(mUBOSubmeshMaterials[aIndex].first); // Submesh material
      }
      else if (UBOTypeIDs::Lights == ubos[i].mTypeID)
      {
        buffers.push_back(mSurface->GetLightManager(mView)->GetUBOLightBuffer()); // Lights
      }
      else if (UBOTypeIDs::Illumination == ubos[i].mTypeID)
      {
        buffers.push_back(mSurface->GetUBOIlluminationBuffer(mView)); // Illumination
      }
      else if (UBOTypeIDs::Model == ubos[i].mTypeID)
      {
        buffers.push_back(mUBOModel); // Model
      }
      else if (UBOTypeIDs::ClipPlanes == ubos[i].mTypeID)
      {
        buffers.push_back(mSurface->GetViewData(mView).mClipPlanesUBO); // Clip Planes
      }
      else
      {
        DebugObjection(true, fmt::format("Failed to find UBO for description name {}\n", ubos[i].mTypeID).c_str());
      }
    }

    mPipelineData.emplace(aSubMesh,
                          aSubMesh->CreatePipelineData(buffers));
  }

  bool VkInstantiatedModel::GetInstanced()
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    return mesh->GetInstanced();
  }

  void VkInstantiatedModel::SetInstanced(bool aInstanced)
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    if (mesh->CanAnimate())
    {
      printf("Currently don't support animating, instanced meshes.");
      return;
    }

    if (mesh->GetInstanced() == aInstanced)
    {
      return;
    }


    mesh->SetInstanced(mMesh);
  }


  void VkInstantiatedModel::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk *aEvent)
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk,
                            this,
                            &VkInstantiatedModel::GraphicsDataUpdateVk);

    auto update = aEvent->mCBO;

    if (mLoadUBOMaterial)
    {
      // TODO: We're updating all materials here, which is unfortunate. Maybe update at the submesh level?
      for (auto material : mUBOSubmeshMaterials)
      {
        material.first->update<UBOMaterial>(0, material.second, update);
      }

      mUBOModelMaterial->update<UBOMaterial>(0, mUBOModelMaterialData, update);

      mLoadUBOMaterial = false;
    }

    if (mLoadUBOAnimation)
    {
      mUBOAnimation->update<UBOAnimation>(0, *mUBOAnimationData, update);
      mLoadUBOAnimation = false;
    }

    if (mLoadUBOModel)
    {
      if (mesh->GetInstanced())
      {
        Instance instance(mUBOModelData);

        mesh->mInstanceManager.InstanceBuffer()->update<Instance>(mesh->GetOffset(this), instance, update);
      }
      else
      {
        mUBOModel->update<UBOModel>(0, mUBOModelData, update);
      }

      mLoadUBOModel = false;
    }
  }
}

