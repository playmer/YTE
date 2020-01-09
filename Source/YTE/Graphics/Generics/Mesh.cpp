#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "glm/gtc/type_ptr.hpp"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/StandardLibrary/File.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  char const* SubmeshData::ToShaderString(TextureType aType)
  {
    switch (aType)
    {
      case SubmeshData::TextureType::Diffuse: return "DIFFUSE";
      case SubmeshData::TextureType::Specular: return "SPECULAR";
      case SubmeshData::TextureType::Ambient: return "AMBIENT";
      case SubmeshData::TextureType::Emissive: return "EMISSIVE";
      case SubmeshData::TextureType::Height: return "HEIGHT";
      case SubmeshData::TextureType::Normal: return "NORMAL";
      case SubmeshData::TextureType::Shininess: return "SHININESS";
      case SubmeshData::TextureType::Opacity: return "OPACITY";
      case SubmeshData::TextureType::Displacment: return "DISPLACEMENT";
      case SubmeshData::TextureType::Lightmap: return "LIGHTMAP";
      case SubmeshData::TextureType::Reflection: return "REFLECTION";
    }

    return "UNKNOWN";
  }

  void CalculateSubMeshDimensions(Submesh& mSubMesh)
  {
    OPTICK_EVENT();

    auto& dimension = mSubMesh.mData.mDimension;

    for (auto const& position : mSubMesh.mData.mVertexData.mPositionData)
    {
      dimension.mMax.x = fmax(position.x, dimension.mMax.x);
      dimension.mMax.y = fmax(position.y, dimension.mMax.y);
      dimension.mMax.z = fmax(position.z, dimension.mMax.z);

      dimension.mMin.x = fmin(position.x, dimension.mMin.x);
      dimension.mMin.y = fmin(position.y, dimension.mMin.y);
      dimension.mMin.z = fmin(position.z, dimension.mMin.z);
    }
  }

  Dimension CalculateDimensions(std::vector<Submesh> const& mParts)
  {
    OPTICK_EVENT();
    Dimension toReturn;

    for (auto& part : mParts)
    {
      auto& dimension = part.mData.mDimension;

      toReturn.mMax.x = fmax(dimension.mMax.x, toReturn.mMax.x);
      toReturn.mMax.y = fmax(dimension.mMax.y, toReturn.mMax.y);
      toReturn.mMax.z = fmax(dimension.mMax.z, toReturn.mMax.z);
      toReturn.mMax.x = fmax(dimension.mMin.x, toReturn.mMax.x);
      toReturn.mMax.y = fmax(dimension.mMin.y, toReturn.mMax.y);
      toReturn.mMax.z = fmax(dimension.mMin.z, toReturn.mMax.z);

      toReturn.mMin.x = fmin(dimension.mMax.x, toReturn.mMin.x);
      toReturn.mMin.y = fmin(dimension.mMax.y, toReturn.mMin.y);
      toReturn.mMin.z = fmin(dimension.mMax.z, toReturn.mMin.z);
      toReturn.mMin.x = fmin(dimension.mMin.x, toReturn.mMin.x);
      toReturn.mMin.y = fmin(dimension.mMin.y, toReturn.mMin.y);
      toReturn.mMin.z = fmin(dimension.mMin.z, toReturn.mMin.z);
    }

    return toReturn;
  }

  YTEDefineType(Mesh)
  {
    RegisterType<Mesh>();
    TypeBuilder<Mesh> builder;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Submesh
  //////////////////////////////////////////////////////////////////////////////
  Submesh::Submesh(SubmeshData&& aRightData)
    : mData{ std::move(aRightData) }
  {
    Initialize();

    UpdateGPUVertexData();
    mIndexBuffer.Update(mData.mIndexData);
  }

  Submesh::Submesh(Submesh&& aRight)
    : mVertexBufferData{ std::move(aRight.mVertexBufferData) }
    , mIndexBuffer{std::move(aRight.mIndexBuffer) }
    , mData{ std::move(aRight.mData) }
  {
  }

  Submesh& Submesh::operator=(Submesh&& aRight)
  {
    mVertexBufferData = std::move(aRight.mVertexBufferData);
    mIndexBuffer = std::move(aRight.mIndexBuffer);
    mData = std::move(aRight.mData);

    return *this;
  }

  void Submesh::Initialize()
  {
    CalculateSubMeshDimensions(*this);
    CreateGPUBuffers();
  }

  void Submesh::UpdateGPUVertexData()
  {
    mVertexBufferData.mPositionBuffer.Update(mData.mVertexData.mPositionData);
    mVertexBufferData.mTextureCoordinatesBuffer.Update(mData.mVertexData.mTextureCoordinatesData);
    mVertexBufferData.mNormalBuffer.Update(mData.mVertexData.mNormalData);
    mVertexBufferData.mColorBuffer.Update(mData.mVertexData.mColorData);
    mVertexBufferData.mTangentBuffer.Update(mData.mVertexData.mTangentData);
    mVertexBufferData.mBinormalBuffer.Update(mData.mVertexData.mBinormalData);
    mVertexBufferData.mBitangentBuffer.Update(mData.mVertexData.mBitangentData);
    mVertexBufferData.mBoneWeightsBuffer.Update(mData.mVertexData.mBoneWeightsData);
    mVertexBufferData.mBoneWeights2Buffer.Update(mData.mVertexData.mBoneWeights2Data);
    mVertexBufferData.mBoneIDsBuffer.Update(mData.mVertexData.mBoneIDsData);
    mVertexBufferData.mBoneIDs2Buffer.Update(mData.mVertexData.mBoneIDs2Data);
  }

  ShaderDescriptions const& Submesh::CreateShaderDescriptions()
  {
    auto& descriptions = mData.mDescriptions;

    if (mData.mDescriptionOverride || mData.mDescriptionsCreated)
    {
      return descriptions;
    }

    auto addUBO = [&descriptions](char const* aName, DescriptorType aDescriptorType, ShaderStageFlags aStage, size_t aBufferSize, size_t aBufferOffset = 0)
    {
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", aName, descriptions.GetBufferBinding()));
      descriptions.AddDescriptor(aDescriptorType, aStage, aBufferSize, aBufferOffset);
    };

    addUBO("VIEW", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::View));
    addUBO("ANIMATION_BONE", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::Animation));
    addUBO("MODEL_MATERIAL", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::Material));
    addUBO("SUBMESH_MATERIAL", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::Material));
    addUBO("LIGHTS", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::LightManager));
    addUBO("ILLUMINATION", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::Illumination));
    addUBO("WATER", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::WaterInformationManager));
    addUBO("MODEL", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::Model));


    // Descriptions for the textures we support based on which maps we found above:
    for (auto sampler : mData.mTextureData)
    {
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", SubmeshData::ToShaderString(sampler.mSamplerType), descriptions.GetBufferBinding()));
      descriptions.AddDescriptor(DescriptorType::CombinedImageSampler, ShaderStageFlags::Fragment, ImageLayout::ShaderReadOnlyOptimal);
    }

    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mPosition;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mTextureCoordinates;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mNormal;
    descriptions.AddBindingAndAttribute<glm::vec4>(VertexInputRate::Vertex, VertexFormat::R32G32B32A32Sfloat); //glm::vec4 mColor;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mTangent;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mBinormal;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mBitangent;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec4 mBoneWeights;
    descriptions.AddBindingAndAttribute<glm::vec2>(VertexInputRate::Vertex, VertexFormat::R32G32Sfloat);       //glm::vec2 mBoneWeights2;
    descriptions.AddBindingAndAttribute<glm::ivec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sint);     //glm::ivec4 mBoneIDs;
    descriptions.AddBindingAndAttribute<glm::ivec2>(VertexInputRate::Vertex, VertexFormat::R32G32Sint);        //glm::ivec4 mBoneIDs;

    mData.mDescriptionsCreated = true;

    return descriptions;
  }

  template <typename T>
  GPUBuffer<T> CreateBuffer(GPUAllocator* aAllocator, size_t aSize)
  {
    return aAllocator->CreateBuffer<T>(aSize,
                                       GPUAllocation::BufferUsage::TransferDst |
                                       GPUAllocation::BufferUsage::VertexBuffer,
                                       GPUAllocation::MemoryProperty::DeviceLocal);
  }

  void Submesh::CreateGPUBuffers()
  {
    auto allocator = mData.mMesh->mRenderer->GetAllocator(AllocatorTypes::Mesh);

    // Create Vertex and Index buffers.
    mVertexBufferData.mPositionBuffer = CreateBuffer<glm::vec3>(allocator, mData.mVertexData.mPositionData.size());
    mVertexBufferData.mTextureCoordinatesBuffer = CreateBuffer<glm::vec3>(allocator, mData.mVertexData.mTextureCoordinatesData.size());
    mVertexBufferData.mNormalBuffer = CreateBuffer<glm::vec3>(allocator, mData.mVertexData.mNormalData.size());
    mVertexBufferData.mColorBuffer = CreateBuffer<glm::vec4>(allocator, mData.mVertexData.mColorData.size());
    mVertexBufferData.mTangentBuffer = CreateBuffer<glm::vec3>(allocator, mData.mVertexData.mTangentData.size());
    mVertexBufferData.mBinormalBuffer = CreateBuffer<glm::vec3>(allocator, mData.mVertexData.mBinormalData.size());
    mVertexBufferData.mBitangentBuffer = CreateBuffer<glm::vec3>(allocator, mData.mVertexData.mBitangentData.size());
    mVertexBufferData.mBoneWeightsBuffer = CreateBuffer<glm::vec3>(allocator, mData.mVertexData.mBoneWeightsData.size());
    mVertexBufferData.mBoneWeights2Buffer = CreateBuffer<glm::vec2>(allocator, mData.mVertexData.mBoneWeights2Data.size());
    mVertexBufferData.mBoneIDsBuffer = CreateBuffer<glm::ivec3>(allocator, mData.mVertexData.mBoneIDsData.size());
    mVertexBufferData.mBoneIDs2Buffer = CreateBuffer<glm::ivec2>(allocator, mData.mVertexData.mBoneIDs2Data.size());
    
    mIndexBuffer = allocator->CreateBuffer<u32>(mData.mIndexData.size(),
                                                GPUAllocation::BufferUsage::TransferDst |
                                                GPUAllocation::BufferUsage::IndexBuffer,
                                                GPUAllocation::MemoryProperty::DeviceLocal);
  }

  void Submesh::ResetTextureCoordinates()
  {
    for (auto&& [textureCoordinate, i] : enumerate(mData.mVertexData.mTextureCoordinatesData))
    {
      *textureCoordinate = mData.mInitialTextureCoordinates[i];
    }
  }

  void Submesh::RecalculateDimensions()
  {
    CalculateSubMeshDimensions(*this);
  }

  void Submesh::UpdatePositionBuffer(std::vector<glm::vec3> const& aData)
  {
    mData.mVertexData.mPositionData = aData;
    mVertexBufferData.mPositionBuffer.Update(mData.mVertexData.mPositionData);

    CalculateSubMeshDimensions(*this);
  }

  void Submesh::UpdateTextureCoordinatesBuffer(std::vector<glm::vec3> const& aData)
  {
    mData.mVertexData.mTextureCoordinatesData = aData;
    mVertexBufferData.mTextureCoordinatesBuffer.Update(mData.mVertexData.mTextureCoordinatesData);
  }

  void Submesh::UpdateNormalBuffer(std::vector<glm::vec3> const& aData)
  {
    mData.mVertexData.mNormalData = aData;
    mVertexBufferData.mNormalBuffer.Update(mData.mVertexData.mNormalData);
  }

  void Submesh::UpdateColorBuffer(std::vector<glm::vec4> const& aData)
  {
    mData.mVertexData.mColorData = aData;
    mVertexBufferData.mColorBuffer.Update(mData.mVertexData.mColorData);
  }

  void Submesh::UpdateTangentBuffer(std::vector<glm::vec3> const& aData)
  {
    mData.mVertexData.mTangentData = aData;
    mVertexBufferData.mTangentBuffer.Update(mData.mVertexData.mTangentData);
  }

  void Submesh::UpdateBinormalBuffer(std::vector<glm::vec3> const& aData)
  {
    mData.mVertexData.mBinormalData = aData;
    mVertexBufferData.mBinormalBuffer.Update(mData.mVertexData.mBinormalData);
  }

  void Submesh::UpdateBitangentBuffer(std::vector<glm::vec3> const& aData)
  {
    mData.mVertexData.mBitangentData = aData;
    mVertexBufferData.mBitangentBuffer.Update(mData.mVertexData.mBitangentData);
  }

  void Submesh::UpdateBoneWeightsBuffer(std::vector<glm::vec3> const& aData)
  {
    mData.mVertexData.mBoneWeightsData = aData;
    mVertexBufferData.mBoneWeightsBuffer.Update(mData.mVertexData.mBoneWeightsData);
  }

  void Submesh::UpdateBoneWeights2Buffer(std::vector<glm::vec2> const& aData)
  {
    mData.mVertexData.mBoneWeights2Data = aData;
    mVertexBufferData.mBoneWeights2Buffer.Update(mData.mVertexData.mBoneWeights2Data);
  }

  void Submesh::UpdateBoneIDsBuffer(std::vector<glm::ivec3> const& aData)
  {
    mData.mVertexData.mBoneIDsData = aData;
    mVertexBufferData.mBoneIDsBuffer.Update(mData.mVertexData.mBoneIDsData);
  }

  void Submesh::UpdateBoneIDs2Buffer(std::vector<glm::ivec2> const& aData)
  {
    mData.mVertexData.mBoneIDs2Data = aData;
    mVertexBufferData.mBoneIDs2Buffer.Update(mData.mVertexData.mBoneIDs2Data);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mesh
  //////////////////////////////////////////////////////////////////////////////
  Mesh::Mesh()
  {

  }

  
  bool ReadMeshFromFile(std::string const& aName, Mesh& aMesh);

  Mesh::Mesh(Renderer *aRenderer,
             const std::string &aFile)
    : mRenderer{aRenderer}
    , mInstanced(false)
  {
    OPTICK_EVENT();

    std::string filename = aFile; // TODO: don't actually make a copy lol
    std::string meshFile;

    // check that the mesh file exists in the game assets folder
    bool success = FileCheck(Path::GetGamePath(), "Models", filename);
    
    if (success)
    {
      // if so, get the model path
      meshFile = Path::GetModelPath(Path::GetGamePath(), aFile);
    }
    else
    {
      // otherwise, it's not in the game assets, so check the engine assets folder
      success = FileCheck(Path::GetEnginePath(), "Models", filename);

      if (success)
      {
        // if it's in the engine assets, get the path
        meshFile = Path::GetModelPath(Path::GetEnginePath(), aFile);
      }
      else
      {
        // otherwise throw an error
        throw "Mesh does not exist.";
      }
    }

    if (ReadMeshFromFile(meshFile, *this))
    {
      return;
    }
  }

  Mesh::Mesh(Renderer* aRenderer,
             std::string const& aFile,
             ContiguousRange<SubmeshData> aSubmeshes)
    : mRenderer{aRenderer}
    , mInstanced(false)
  {
    OPTICK_EVENT();
    mName = aFile;

    mParts.reserve(aSubmeshes.size());

    for (auto& submeshData : aSubmeshes)
    {
      submeshData.mMesh = this;
      auto& submesh = mParts.emplace_back(std::move(submeshData));

      CalculateSubMeshDimensions(submesh);
    }

    mDimension = CalculateDimensions(mParts);
  }

  void Mesh::RecalculateDimensions()
  {
    OPTICK_EVENT();

    mDimension = CalculateDimensions(mParts);
  }

  Mesh::~Mesh()
  {

  }

  bool Mesh::CanAnimate()
  {
    return mSkeleton.HasBones();
  }

  std::vector<Submesh>& Mesh::GetSubmeshes()
  {
    return mParts;
  }

  void Mesh::SetBackfaceCulling(bool aCulling)
  {
    for (auto &sub : mParts)
    {
      sub.mData.mCullBackFaces = aCulling;
    }
  }

  void Mesh::ResetTextureCoordinates()
  {
    OPTICK_EVENT();

    for (auto& submesh : mParts)
    {
      submesh.ResetTextureCoordinates();
    }
  }

  void ReadSkeletonFromFile(std::string const& aName, Skeleton& aSkeleton)
  {
    std::string fileName = aName + ".YTESkeleton";

    FileReader file{ fileName };

    if (file.mOpened)
    {
      SkeletonHeader skeletonHeader = file.Read<SkeletonHeader>();
        
      aSkeleton.mBoneData.resize(static_cast<size_t>(skeletonHeader.mBoneDataSize));
      aSkeleton.mVertexSkeletonData.resize(static_cast<size_t>(skeletonHeader.mVertexSkeletonDataSize));

      aSkeleton.mGlobalInverseTransform = file.Read<glm::mat4>();
      aSkeleton.mDefaultOffsets = file.Read<UBOs::Animation>();

      file.Read(aSkeleton.mBoneData.data(), aSkeleton.mBoneData.size());
      file.Read(aSkeleton.mVertexSkeletonData.data(), aSkeleton.mVertexSkeletonData.size());

      for (u64 i = 0 ; i < skeletonHeader.mBoneMappingSize; ++i)
      {
        auto bone = file.Read<u64>();
        auto keySize = static_cast<size_t>(file.Read<u64>());

        std::string boneName;
        boneName.resize(keySize);
        file.Read<>(boneName.data(), keySize);

        aSkeleton.mBones.emplace(std::move(boneName), static_cast<u32>(bone));
      }
    }
  }

  bool ReadMeshFromFile(std::string const& aName, Mesh& aMesh)
  {
    OPTICK_EVENT();
    std::string fileName = aName + ".YTEMesh";
    
    FileReader file{ fileName };

    if (!file.mOpened)
    {
      return false;
    }

    auto meshHeader = file.Read<MeshHeader>();
    aMesh.mDimension = file.Read<Dimension>();
    aMesh.mParts.resize(static_cast<size_t>(meshHeader.mNumberOfSubmeshes));

    for (u64 i = 0; i < meshHeader.mNumberOfSubmeshes; ++i)
    {
      auto& submesh = aMesh.mParts[static_cast<size_t>(i)];
      auto& submeshData = submesh.mData;
      auto& vertexData = submeshData.mVertexData;

      auto submeshHeader = file.Read<SubmeshHeader>();

      submeshData.mName.resize(submeshHeader.mNameSize);
      submeshData.mMaterialName.resize(submeshHeader.mMaterialNameSize);
      submeshData.mShaderSetName.resize(submeshHeader.mShaderSetNameSize);
      vertexData.mPositionData.resize(submeshHeader.mNumberOfPositions);
      vertexData.mTextureCoordinatesData.resize(submeshHeader.mNumberOfTextureCoordinates);
      vertexData.mNormalData.resize(submeshHeader.mNumberOfNormals);
      vertexData.mColorData.resize(submeshHeader.mNumberOfColors);
      vertexData.mTangentData.resize(submeshHeader.mNumberOfTangents);
      vertexData.mBinormalData.resize(submeshHeader.mNumberOfBinormals);
      vertexData.mBitangentData.resize(submeshHeader.mNumberOfBitangents);
      vertexData.mBoneWeightsData.resize(submeshHeader.mNumberOfBoneWeights);
      vertexData.mBoneWeights2Data.resize(submeshHeader.mNumberOfBoneWeights2);
      vertexData.mBoneIDsData.resize(submeshHeader.mNumberOfBoneIds);
      vertexData.mBoneIDs2Data.resize(submeshHeader.mNumberOfBoneIds2);
      submeshData.mIndexData.resize(submeshHeader.mNumberOfIndices);
      submeshData.mTextureData.resize(submeshHeader.mNumberOfTextures);

      file.Read(submeshData.mName.data(), submeshData.mName.size());
      file.Read(submeshData.mMaterialName.data(), submeshData.mMaterialName.size());
      file.Read(submeshData.mShaderSetName.data(), submeshData.mShaderSetName.size());

      file.Read(vertexData.mPositionData.data(), vertexData.mPositionData.size());
      file.Read(vertexData.mTextureCoordinatesData.data(), vertexData.mTextureCoordinatesData.size());
      file.Read(vertexData.mNormalData.data(), vertexData.mNormalData.size());
      file.Read(vertexData.mColorData.data(), vertexData.mColorData.size());
      file.Read(vertexData.mTangentData.data(), vertexData.mTangentData.size());
      file.Read(vertexData.mBinormalData.data(), vertexData.mBinormalData.size());
      file.Read(vertexData.mBitangentData.data(), vertexData.mBitangentData.size());
      file.Read(vertexData.mBoneWeightsData.data(), vertexData.mBoneWeightsData.size());
      file.Read(vertexData.mBoneWeights2Data.data(), vertexData.mBoneWeights2Data.size());
      file.Read(vertexData.mBoneIDsData.data(), vertexData.mBoneIDsData.size());
      file.Read(vertexData.mBoneIDs2Data.data(), vertexData.mBoneIDs2Data.size());
      file.Read(submeshData.mIndexData.data(), submeshData.mIndexData.size());

      submeshData.mUBOMaterial =  file.Read<UBOs::Material>();
      submeshData.mDimension = file.Read<Dimension>();
        
      for (auto& texture : submeshData.mTextureData)
      {
        auto textureDataHeader = file.Read<TextureDataHeader>();
        texture.mSamplerType = textureDataHeader.mSamplerType;
        texture.mViewType = textureDataHeader.mViewType;

        texture.mName.resize(textureDataHeader.mStringSize);
        file.Read(texture.mName.data(), texture.mName.size());
      }

      submeshData.mInitialTextureCoordinates = vertexData.mTextureCoordinatesData;
        
      submeshData.mMesh = &aMesh;
      submesh.CreateGPUBuffers();
      submesh.UpdateGPUVertexData();
      submesh.mIndexBuffer.Update(submeshData.mIndexData);
    }

    if (meshHeader.mHasSkeleton)
    {
      ReadSkeletonFromFile(aName, aMesh.mSkeleton);
    }

    return true;
  }
}
