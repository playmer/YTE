#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "glm/gtc/type_ptr.hpp"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  char const* ToShaderString(SubmeshData::TextureType aType)
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

  static inline
  glm::vec3 ToGlm(const aiVector3D *aVector)
  {
    return { aVector->x, aVector->y ,aVector->z };
  }

  static inline
  glm::vec3 ToGlm(const aiColor3D *aVector)
  {
    return { aVector->r, aVector->g ,aVector->b };
  }

  static inline
  glm::quat ToGlm(const aiQuaternion *aQuat)
  {
    glm::quat quaternion;

    quaternion.x = aQuat->x;
    quaternion.y = aQuat->y;
    quaternion.z = aQuat->z;
    quaternion.w = aQuat->w;

    return quaternion;
  }

  static inline
  glm::mat4 ToGlm(const aiMatrix4x4 &aMatrix)
  {
    return glm::transpose(glm::make_mat4(&aMatrix.a1));
  }

  static inline
  aiMatrix4x4 ToAssimp(const glm::mat4 &aMatrix)
  {
    auto transposed = glm::transpose(aMatrix);
    return *(reinterpret_cast<aiMatrix4x4*>(glm::value_ptr(transposed)));
  }

  void CalculateSubMeshDimensions(Submesh& mSubMesh)
  {
    YTEProfileFunction();

    auto& dimension = mSubMesh.mData.mDimension;

    for (auto const& vertex : mSubMesh.mData.mVertexData)
    {
      dimension.mMax.x = fmax(vertex.mPosition.x, dimension.mMax.x);
      dimension.mMax.y = fmax(vertex.mPosition.y, dimension.mMax.y);
      dimension.mMax.z = fmax(vertex.mPosition.z, dimension.mMax.z);

      dimension.mMin.x = fmin(vertex.mPosition.x, dimension.mMin.x);
      dimension.mMin.y = fmin(vertex.mPosition.y, dimension.mMin.y);
      dimension.mMin.z = fmin(vertex.mPosition.z, dimension.mMin.z);
    }
  }

  Dimension CalculateDimensions(std::vector<Submesh> const& mParts)
  {
    YTEProfileFunction();
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
  // Skeleton
  //////////////////////////////////////////////////////////////////////////////
  bool Skeleton::HasBones(aiScene const* aScene)
  {
    u32 const numMeshes = aScene->mNumMeshes;

    for (u32 i = 0; i < numMeshes; ++i)
    {
      aiMesh const* mesh = aScene->mMeshes[i];

      if (0 < mesh->mNumBones)
      {
        return true;
      }
    }

    return false;
  }

  void Skeleton::Initialize(aiScene const* aScene)
  {
    YTEProfileFunction();
    // find number of vertices to initialize the skeleton
    u32 numMeshes = aScene->mNumMeshes;
    u32 vertCount = 0;
    for (u32 i = 0; i < numMeshes; ++i)
    {
      vertCount += aScene->mMeshes[i]->mNumVertices;
    }

    mVertexSkeletonData.resize(vertCount);
//#ifdef _DEBUG
//    mVertexErrorAdds.resize(vertCount);
//#endif

    mNumBones = 0;

    auto globalInverse = aScene->mRootNode->mTransformation;
    mGlobalInverseTransform = ToGlm(globalInverse.Inverse());

    u32 startingVertex = 0;
    for (u32 i = 0; i < numMeshes; ++i)
    {
      aiMesh* mesh = aScene->mMeshes[i];

      if (0 < mesh->mNumBones)
      {
        LoadBoneData(mesh, startingVertex);
      }

      startingVertex += mesh->mNumVertices;
    }

    PreTransform(aScene);
  }



  void Skeleton::LoadBoneData(const aiMesh* aMesh, uint32_t aVertexStartingIndex)
  {
    YTEProfileFunction();
    DebugObjection(aMesh->mNumBones >= BoneConstants::MaxBones,
                   "Animated models cannot have more than %d bones, %s mesh has %d bones.",
                   BoneConstants::MaxBones, aMesh->mName.C_Str(), aMesh->mNumBones);

    for (uint32_t i = 0; i < aMesh->mNumBones; ++i)
    {
      uint32_t index = 0;

      auto bone = aMesh->mBones[i];
      std::string boneName(bone->mName.data);

      // try to find bone by name
      auto boneIt = mBones.find(boneName);
      if (boneIt == mBones.end())
      {
        // insert new bone
        index = mNumBones;
        ++mNumBones;
        mBones[boneName] = index;
        mBoneData.emplace_back(ToGlm(bone->mOffsetMatrix));
      }
      else
      {
        // use existing bone
        index = boneIt->second;
      }

      // get the weights and vertex IDs from the bone data
      for (uint32_t b = 0; b < bone->mNumWeights; ++b)
      {
        auto weight = bone->mWeights[b];
        uint32_t id = aVertexStartingIndex + weight.mVertexId;
//#ifdef _DEBUG
//        if (mVertexSkeletonData[id].AddBone(index, weight.mWeight) == false)
//        {
//          mVertexErrorAdds[id] += 1;
//        }
//#else
        mVertexSkeletonData[id].AddBone(index, weight.mWeight);
//#endif
      }
    }

    // loop errors
//#if defined(_DEBUG) && defined(BSTDULAF)
//    for (size_t i = 0; i < mVertexErrorAdds.size(); ++i)
//    {
//      if (mVertexErrorAdds[i] != 0)
//      {
//        std::cout << "Warning: Vertex " << i << " did not add " << mVertexErrorAdds[i] << " of extra bones to the skeleton." << std::endl;
//        std::cout << "\t\tVertex Bone Information: \n\t\t\tWeights: "
//                  << mVertexSkeletonData[i].mWeights[0] << ",\t"
//                  << mVertexSkeletonData[i].mWeights[1] << ",\t"
//                  << mVertexSkeletonData[i].mWeights[2] << ",\t"
//                  << mVertexSkeletonData[i].mWeights[3] << "\n\t\t\t     IDs: "
//                  << mVertexSkeletonData[i].mWeights[4] << "\n\t\t\t     IDs: "
//                  << mVertexSkeletonData[i].mIDs[0] << ",\t"
//                  << mVertexSkeletonData[i].mIDs[1] << ",\t"
//                  << mVertexSkeletonData[i].mIDs[2] << ",\t"
//                  << mVertexSkeletonData[i].mIDs[3] << "\t"
//                  << mVertexSkeletonData[i].mIDs[4] << "\n\n";
//
//      }
//    }
//#endif
  }



  void Skeleton::PreTransform(const aiScene* aScene)
  {
    // setup parent transforms
    glm::mat4 identity;

    // recursive step
    VisitNodes(aScene->mRootNode, identity);

    mDefaultOffsets.mHasAnimation = 1;
    for (uint32_t i = 0; i < mNumBones; ++i)
    {
      mDefaultOffsets.mBones[i] = mBoneData[i].mFinalTransformation;
    }
  }



  void Skeleton::VisitNodes(const aiNode* aNode, glm::mat4 const& aParentTransform)
  {
    std::string nodeName(aNode->mName.data);
    glm::mat4 globalTrans = aParentTransform * ToGlm(aNode->mTransformation);

    auto index = mBones.find(nodeName);

    if (index != mBones.end())
    {
      mBoneData[index->second].mFinalTransformation = 
        mGlobalInverseTransform *
        globalTrans *
        mBoneData[index->second].mOffset;
    }

    for (uint32_t i = 0; i < aNode->mNumChildren; ++i)
    {
      VisitNodes(aNode->mChildren[i], globalTrans);
    }
  }

  SubmeshData::TextureType ToYTE(aiTextureType aType)
  {
    switch (aType)
    {
      case aiTextureType_DIFFUSE: return SubmeshData::TextureType::Diffuse;
      case aiTextureType_SPECULAR: return SubmeshData::TextureType::Specular;
      case aiTextureType_AMBIENT: return SubmeshData::TextureType::Ambient;
      case aiTextureType_EMISSIVE: return SubmeshData::TextureType::Emissive;
      case aiTextureType_HEIGHT: return SubmeshData::TextureType::Height;
      case aiTextureType_NORMALS: return SubmeshData::TextureType::Normal;
      case aiTextureType_SHININESS: return SubmeshData::TextureType::Shininess;
      case aiTextureType_OPACITY: return SubmeshData::TextureType::Opacity;
      case aiTextureType_DISPLACEMENT: return SubmeshData::TextureType::Displacment;
      case aiTextureType_LIGHTMAP: return SubmeshData::TextureType::Lightmap;
      case aiTextureType_REFLECTION: return SubmeshData::TextureType::Reflection;
    }

    return SubmeshData::TextureType::Unknown;
  }



  //////////////////////////////////////////////////////////////////////////////
  // Submesh
  //////////////////////////////////////////////////////////////////////////////
  Submesh::Submesh(Renderer* aRenderer,
    Mesh* aYTEMesh,
    const aiScene* aScene,
    const aiMesh* aMesh,
    Skeleton* aSkeleton,
    uint32_t aBoneStartingVertexOffset)
  {
    YTEProfileFunction();

    mData.mMesh = aYTEMesh;

    aiColor3D pColor(0.f, 0.f, 0.f);
    aScene->mMaterials[aMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE,
      pColor);

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    auto material = aScene->mMaterials[aMesh->mMaterialIndex];

    aiString name;

    aiColor3D Diffuse;
    aiColor3D Ambient;
    aiColor3D Specular;
    aiColor3D Emissive;
    aiColor3D Transparent;
    aiColor3D Reflective;

    material->Get(AI_MATKEY_COLOR_DIFFUSE, Diffuse);
    material->Get(AI_MATKEY_COLOR_AMBIENT, Ambient);
    material->Get(AI_MATKEY_COLOR_SPECULAR, Specular);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, Emissive);
    material->Get(AI_MATKEY_COLOR_TRANSPARENT, Transparent);
    material->Get(AI_MATKEY_COLOR_REFLECTIVE, Reflective);

    auto& uboMaterial = mData.mUBOMaterial;

    uboMaterial.mDiffuse = glm::vec4(ToGlm(&Diffuse), 1.0f);
    uboMaterial.mAmbient = glm::vec4(ToGlm(&Ambient), 1.0f);
    uboMaterial.mSpecular = glm::vec4(ToGlm(&Specular), 1.0f);
    uboMaterial.mEmissive = glm::vec4(ToGlm(&Emissive), 1.0f);
    uboMaterial.mTransparent = glm::vec4(ToGlm(&Transparent), 1.0f);
    uboMaterial.mReflective = glm::vec4(ToGlm(&Reflective), 1.0f);
    uboMaterial.mFlags = 0;

    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_OPACITY, uboMaterial.mOpacity);
    material->Get(AI_MATKEY_SHININESS, uboMaterial.mShininess);
    material->Get(AI_MATKEY_SHININESS_STRENGTH, uboMaterial.mShininessStrength);
    material->Get(AI_MATKEY_REFLECTIVITY, uboMaterial.mReflectivity);
    material->Get(AI_MATKEY_REFRACTI, uboMaterial.mReflectiveIndex);
    material->Get(AI_MATKEY_BUMPSCALING, uboMaterial.mBumpScaling);

    mData.mName = aMesh->mName.C_Str();
    mData.mMaterialName = name.C_Str();

    std::array textureTypesSupported{
      // The texture is combined with the result of the diffuse lighting equation.
      aiTextureType_DIFFUSE,

      // The texture is combined with the result of the specular lighting equation.
      aiTextureType_SPECULAR,

      //// The texture is combined with the result of the ambient lighting equation.
      //aiTextureType_AMBIENT,

      //// The texture is added to the result of the lighting calculation. It isn't 
      //// influenced by incoming light.
      //aiTextureType_EMISSIVE,

      //// Height Map: By convention, higher gray-scale values stand for higher elevations 
      //// from the base height.
      //aiTextureType_HEIGHT,

      // Normal Map: (Tangent Space) Again, there are several conventions for tangent-space 
      // normal maps. Assimp does (intentionally) not distinguish here.
      aiTextureType_NORMALS,

      //// The texture defines the glossiness of the material.
      //// The glossiness is in fact the exponent of the specular (phong) lighting 
      //// equation. Usually there is a conversion function defined to map the linear
      //// color values in the texture to a suitable exponent. Have fun.
      //aiTextureType_SHININESS,

      //// The texture defines per-pixel opacity. Usually 'white' means opaque and 
      //// 'black' means 'transparency'. Or quite the opposite. Have fun.
      //aiTextureType_OPACITY,

      //// Displacement texture: The exact purpose and format is application-dependent. 
      //// Higher color values stand for higher vertex displacements.
      //aiTextureType_DISPLACEMENT,

      //// Lightmap texture: (aka Ambient Occlusion) Both 'Lightmaps' and dedicated 
      //// 'ambient occlusion maps' are covered by this material property. The texture contains 
      //// a scaling value for the final color value of a pixel. Its intensity is not affected by 
      //// incoming light.
      //aiTextureType_LIGHTMAP,

      //// Reflection texture: Contains the color of a perfect mirror reflection. 
      //// Rarely used, almost never for real-time applications.
      //aiTextureType_REFLECTION
    };


    std::string defaultTexture{ "white.png" };

    for (auto type : textureTypesSupported)
    {
      aiString aiTextureName;
      material->GetTexture(type, 0, &aiTextureName);

      std::string textureName;

      if (0 != aiTextureName.length)
      {
        textureName = aiTextureName.C_Str();

        if (aiTextureType_NORMALS == type)
        {
          uboMaterial.mUsesNormalTexture = 1;
        }
      }
      else
      {
        textureName = defaultTexture;
      }

      mData.mTextureData.emplace_back(textureName, TextureViewType::e2D, ToYTE(type));

      aRenderer->RequestTexture(textureName);
    }

    mData.mShaderSetName = "Phong";

    // get the vertex data with bones (if provided)
    for (unsigned int j = 0; j < aMesh->mNumVertices; j++)
    {
      const aiVector3D *pPos = aMesh->mVertices + j;
      const aiVector3D *pNormal = aMesh->mNormals + j;
      const aiVector3D *pTexCoord = &Zero3D;
      const aiVector3D *pTangent = &Zero3D;
      const aiVector3D *pBiTangent = &Zero3D;

      if (aMesh->HasTextureCoords(0))
      {
        pTexCoord = aMesh->mTextureCoords[0] + j;
      }

      if (aMesh->HasTangentsAndBitangents())
      {
        pTangent = aMesh->mTangents + j;
        pBiTangent = aMesh->mBitangents + j;
      }

      auto position = ToGlm(pPos);

      // NOTE: We do this to invert the uvs to what the texture would expect.
      auto textureCoordinates = glm::vec3{ pTexCoord->x,
                                           1.0f - pTexCoord->y,
                                           pTexCoord->z };

      auto normal = ToGlm(pNormal);
      auto color = glm::vec4{ ToGlm(&pColor), 1.0f };
      auto tangent = ToGlm(pTangent);
      auto binormal = glm::cross(tangent, normal);
      auto bitangent = ToGlm(pBiTangent);

      glm::vec3 boneWeights;
      glm::vec2 boneWeights2;
      glm::ivec3 boneIDs;
      glm::ivec2 boneIDs2;

      if (aSkeleton->HasBones())
      {
        auto vertexData = aSkeleton->GetVertexBoneData()[aBoneStartingVertexOffset + j];

        // has bones, now we find the weights for this vertex
        for (uint32_t i = 0; i < BoneConstants::MaxBonesPerVertex1; ++i)
        {
          boneWeights[i] = vertexData.mWeights[i];
          boneIDs[i] = vertexData.mIDs[i];
        }

        for (uint32_t i = 0; i < BoneConstants::MaxBonesPerVertex2; ++i)
        {
          boneWeights2[i] = vertexData.mWeights[BoneConstants::MaxBonesPerVertex1 + i];
          boneIDs2[i] = vertexData.mIDs[BoneConstants::MaxBonesPerVertex1 + i];
        }
      }
      else
      {
        // no bones, so default weights
        boneWeights = glm::vec3(0.0f, 0.0f, 0.0f);
        boneWeights2 = glm::vec2(0.0f, 0.0f);
        boneIDs = glm::ivec3(0, 0, 0);
        boneIDs2 = glm::ivec2(0, 0);
      }
      
      mData.mVertexData.emplace_back(
        position,
        textureCoordinates,
        normal,
        color,
        tangent,
        binormal,
        bitangent,
        boneWeights,
        boneWeights2,
        boneIDs,
        boneIDs2);

      mData.mInitialTextureCoordinates.emplace_back(textureCoordinates);
    }

    uint32_t indexBase = static_cast<uint32_t>(mData.mIndexData.size());

    for (unsigned int j = 0; j < aMesh->mNumFaces; j++)
    {
      const aiFace &Face = aMesh->mFaces[j];

      if (Face.mNumIndices != 3)
      {
        continue;
      }

      mData.mIndexData.push_back(indexBase + Face.mIndices[0]);
      mData.mIndexData.push_back(indexBase + Face.mIndices[1]);
      mData.mIndexData.push_back(indexBase + Face.mIndices[2]);
    }

    Initialize();

    mVertexBuffer.Update(mData.mVertexData);
    mIndexBuffer.Update(mData.mIndexData);
  }

  Submesh::Submesh(SubmeshData&& aRightData)
    : mData{ std::move(aRightData) }
  {
    Initialize();

    mVertexBuffer.Update(mData.mVertexData);
    mIndexBuffer.Update(mData.mIndexData);
  }

  Submesh::Submesh(Submesh&& aRight)
    : mVertexBuffer{ std::move(aRight.mVertexBuffer) }
    , mIndexBuffer{std::move(aRight.mIndexBuffer) }
    , mData{ std::move(aRight.mData) }
  {
  }

  Submesh& Submesh::operator=(Submesh&& aRight)
  {
    mVertexBuffer = std::move(aRight.mVertexBuffer);
    mIndexBuffer = std::move(aRight.mIndexBuffer);
    mData = std::move(aRight.mData);

    return *this;
  }

  void Submesh::Initialize()
  {
    CalculateSubMeshDimensions(*this);
    CreateGPUBuffers();
  }


  ShaderDescriptions const& Submesh::CreateShaderDescriptions()
  {
    auto& descriptions = mData.mDescriptions;

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


    // Descriptions for the textures we support based on which maps we found above:
    for (auto sampler : mData.mTextureData)
    {
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", ToShaderString(sampler.mSamplerType), descriptions.GetBufferBinding()));
      descriptions.AddDescriptor(DescriptorType::CombinedImageSampler, ShaderStageFlags::Fragment, ImageLayout::ShaderReadOnlyOptimal);
    }

    descriptions.AddBinding<Vertex>(VertexInputRate::Vertex);
    descriptions.AddAttribute<glm::vec3>(VertexFormat::R32G32B32Sfloat);    //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(VertexFormat::R32G32B32Sfloat);    //glm::vec3 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec3>(VertexFormat::R32G32B32Sfloat);    //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec4>(VertexFormat::R32G32B32A32Sfloat); //glm::vec4 mColor;
    descriptions.AddAttribute<glm::vec3>(VertexFormat::R32G32B32Sfloat);    //glm::vec3 mTangent;
    descriptions.AddAttribute<glm::vec3>(VertexFormat::R32G32B32Sfloat);    //glm::vec3 mBinormal;
    descriptions.AddAttribute<glm::vec3>(VertexFormat::R32G32B32Sfloat);    //glm::vec3 mBitangent;
    descriptions.AddAttribute<glm::vec3>(VertexFormat::R32G32B32Sfloat);    //glm::vec4 mBoneWeights;
    descriptions.AddAttribute<glm::vec2>(VertexFormat::R32G32Sfloat);       //glm::vec2 mBoneWeights2;
    descriptions.AddAttribute<glm::ivec3>(VertexFormat::R32G32B32Sint);     //glm::ivec4 mBoneIDs;
    descriptions.AddAttribute<glm::ivec2>(VertexFormat::R32G32Sint);        //glm::ivec4 mBoneIDs;

    // Model Buffer for Vertex shader. (Non-instanced Meshes)
    addUBO("MODEL", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::Model));

    return descriptions;
  }

  void Submesh::CreateGPUBuffers()
  {
    auto allocator = mData.mMesh->mRenderer->GetAllocator(AllocatorTypes::Mesh);

    // Create Vertex, Index, and Material buffers.
    mVertexBuffer = allocator->CreateBuffer<Vertex>(mData.mVertexData.size(),
                                                    GPUAllocation::BufferUsage::TransferDst |
                                                    GPUAllocation::BufferUsage::VertexBuffer,
                                                    GPUAllocation::MemoryProperty::DeviceLocal);

    mIndexBuffer = allocator->CreateBuffer<u32>(mData.mIndexData.size(),
                                                GPUAllocation::BufferUsage::TransferDst |
                                                GPUAllocation::BufferUsage::IndexBuffer,
                                                GPUAllocation::MemoryProperty::DeviceLocal);
  }

  void Submesh::ResetTextureCoordinates()
  {
    for (auto&& [vertex, i] : enumerate(mData.mVertexData))
    {
      vertex->mTextureCoordinates = mData.mInitialTextureCoordinates[i];
    }
  }

  void Submesh::UpdateVertices(std::vector<Vertex> const& aVertices)
  {
    DebugObjection(aVertices.size() != mData.mVertexData.size(), 
                   "UpdateVerticesAndIndices cannot change the size of the vertex buffer from %i to %i", 
                   mData.mVertexData.size(), 
                   aVertices.size());

    mData.mVertexData = aVertices;

    mVertexBuffer.Update(mData.mVertexData);

    CalculateSubMeshDimensions(*this);
  }

  void Submesh::UpdateVerticesAndIndices(std::vector<Vertex> const& aVertices, std::vector<u32> const& aIndices)
  {
    DebugObjection(aVertices.size() != mData.mVertexData.size(),
                   "UpdateVerticesAndIndices cannot change the size of the vertex buffer from %i to %i", 
                   mData.mVertexData.size(), 
                   aVertices.size());

    DebugObjection(aIndices.size() != mData.mIndexData.size(),
                   "UpdateVerticesAndIndices cannot change the size of the index buffer from %i to %i", 
                   mData.mIndexData.size(), 
                   aIndices.size());

    mData.mVertexData = aVertices;
    mData.mIndexData = aIndices;

    mVertexBuffer.Update(mData.mVertexData);
    mIndexBuffer.Update(mData.mIndexData);

    CalculateSubMeshDimensions(*this);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mesh
  //////////////////////////////////////////////////////////////////////////////
  Mesh::Mesh()
  {

  }

  Mesh::Mesh(Renderer *aRenderer,
             const std::string &aFile)
    : mRenderer{aRenderer}
    , mInstanced(false)
  {
    YTEProfileFunction();
    Assimp::Importer Importer;

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

    auto meshScene = Importer.ReadFile(meshFile.c_str(),
      aiProcess_Triangulate |
      aiProcess_CalcTangentSpace |
      aiProcess_GenSmoothNormals);

    if (meshScene)
    {
      // If this scene has bones, we need to reload it without pre-transforming the
      // vertices.
      if (false == Skeleton::HasBones(meshScene))
      {
        Importer.ApplyPostProcessing(
          //aiProcess_Triangulate |
          //aiProcess_GenSmoothNormals |
          //aiProcess_CalcTangentSpace |
          aiProcess_PreTransformVertices
        );
      }
      else
      {
        // Load bone data
        mSkeleton.Initialize(meshScene);
      }

      mParts.clear();
      mParts.reserve(meshScene->mNumMeshes);

      //printf("Mesh FileName: %s\n", aFile.c_str());

      u32 numMeshes = meshScene->mNumMeshes;

      // Load Mesh
      u32 startingVertex = 0;
      for (u32 i = 0; i < numMeshes; i++)
      {
        mParts.emplace_back(aRenderer,
                            this,
                            meshScene, 
                            meshScene->mMeshes[i], 
                            &mSkeleton, 
                            startingVertex);
        startingVertex += meshScene->mMeshes[i]->mNumVertices;
      }

      mName = aFile;
    }

    mDimension = CalculateDimensions(mParts);
  }

  Mesh::Mesh(Renderer* aRenderer,
             std::string const& aFile,
             ContiguousRange<SubmeshData> aSubmeshes)
    : mRenderer{aRenderer}
    , mInstanced(false)
  {
    YTEProfileFunction();
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

  void Mesh::UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex> const& aVertices)
  {
    DebugObjection(
      aVertices.size() != mParts[aSubmeshIndex].mData.mVertexData.size(), 
      "UpdateVertices cannot change the size of the vertex buffer from %i to %i", 
      mParts[aSubmeshIndex].mData.mVertexData.size(), 
      aVertices.size());


    mParts[aSubmeshIndex].UpdateVertices(aVertices);

    mDimension = CalculateDimensions(mParts);
  }

  void Mesh::UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex> const& aVertices, std::vector<u32> const& aIndices)
  {
    mParts[aSubmeshIndex].UpdateVerticesAndIndices(aVertices, aIndices);

    mDimension = CalculateDimensions(mParts);
  }

  Mesh::~Mesh()
  {

  }

  bool Mesh::CanAnimate()
  {
    return mSkeleton.HasBones();
  }

  std::vector<YTE::Submesh>& Mesh::GetSubmeshes()
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
    for (auto& submesh : mParts)
    {
      submesh.ResetTextureCoordinates();
    }
  }
}
