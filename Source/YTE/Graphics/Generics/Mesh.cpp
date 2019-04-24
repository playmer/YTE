///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

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
  bool Skeleton::Initialize(const aiScene* aScene)
  {
    YTEProfileFunction();
    // find number of vertices to initialize the skeleton
    uint32_t numMeshes = aScene->mNumMeshes;
    uint32_t vertCount = 0;
    for (uint32_t i = 0; i < numMeshes; ++i)
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

    bool bonesFound = false;
    uint32_t startingVertex = 0;
    for (uint32_t i = 0; i < numMeshes; ++i)
    {
      aiMesh *m = aScene->mMeshes[i];
      if (m->mNumBones > 0)
      {
        LoadBoneData(m, startingVertex);
        bonesFound = true;
      }
      startingVertex += m->mNumVertices;
    }

    if (bonesFound)
    {
      PreTransform(aScene);
    }

    return bonesFound;
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



  //////////////////////////////////////////////////////////////////////////////
  // Submesh
  //////////////////////////////////////////////////////////////////////////////
  Submesh::Submesh(Renderer *aRenderer,
                   Mesh* aYTEMesh,
                   const aiScene *aScene,
                   const aiMesh *aMesh,
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

    // TODO (Andrew): Add ability to provide a shader if wanted
    //mShaderSetName = mMaterialName.substr(0, mMaterialName.find_first_of('_'));

    aiString diffuse;
    aiString specular;
    aiString normals;

    material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse);
    material->GetTexture(aiTextureType_SPECULAR, 0, &specular);
    material->GetTexture(aiTextureType_NORMALS, 0, &normals);

    std::string defaultTexture{ "white.png" };

    mData.mDiffuseMap = defaultTexture;
    mData.mSpecularMap = defaultTexture;
    mData.mNormalMap = defaultTexture;

    if (0 != diffuse.length)
    {
      mData.mDiffuseMap = diffuse.C_Str();
    }

    if (0 != specular.length)
    {
      mData.mSpecularMap = specular.C_Str();
    }

    if (0 != normals.length)
    {
      mData.mNormalMap = normals.C_Str();
      uboMaterial.mUsesNormalTexture = 1; // true
    }

    aRenderer->RequestTexture(mData.mDiffuseMap);
    aRenderer->RequestTexture(mData.mSpecularMap);
    aRenderer->RequestTexture(mData.mNormalMap);

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
  }

  Submesh::Submesh(SubmeshData&& aRightData)
    : mData{ std::move(aRightData) }
  {
    Initialize();
  }

  Submesh::Submesh(Submesh&& aRight)
    : mVertexBuffer{ std::move(aRight.mVertexBuffer) }
    , mIndexBuffer{std::move(aRight.mIndexBuffer) }
    , mData{ std::move(aRight.mData) }
  {
    Initialize();
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

    mVertexBuffer.Update(mData.mVertexData.data(), mData.mVertexData.size());
    mIndexBuffer.Update(mData.mIndexData.data(), mData.mIndexData.size());
  }

  void Submesh::ResetTextureCoordinates()
  {
    for (auto&& [vertex, i] : enumerate(mData.mVertexData))
    {
      vertex->mTextureCoordinates = mData.mInitialTextureCoordinates[i];
    }
  }

  void Submesh::UpdateVertices(std::vector<Vertex>& aVertices)
  {
    DebugObjection(aVertices.size() != mData.mVertexData.size(), 
                   "UpdateVerticesAndIndices cannot change the size of the vertex buffer from %i to %i", 
                   mData.mVertexData.size(), 
                   aVertices.size());

    mData.mVertexData = aVertices;

    mVertexBuffer.Update(mData.mVertexData.data(), mData.mVertexData.size());

    CalculateSubMeshDimensions(*this);
  }

  void Submesh::UpdateVerticesAndIndices(std::vector<Vertex>& aVertices, std::vector<u32>& aIndices)
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

    mVertexBuffer.Update(mData.mVertexData.data(), mData.mVertexData.size());
    mIndexBuffer.Update(mData.mIndexData.data(), mData.mIndexData.size());

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
    Assimp::Importer ImporterCol;

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

    auto pScene = Importer.ReadFile(meshFile.c_str(),
      aiProcess_Triangulate |
      //aiProcess_PreTransformVertices |
      aiProcess_CalcTangentSpace |
      aiProcess_GenSmoothNormals);

    auto pColliderScene = ImporterCol.ReadFile(meshFile.c_str(),
      aiProcess_Triangulate |
      aiProcess_PreTransformVertices |
      aiProcess_CalcTangentSpace |
      aiProcess_GenSmoothNormals);

    if (pScene)
    {
      // Load bone data
      bool hasBones = mSkeleton.Initialize(pScene);
      
      auto pMeshScene = pScene;
      if (!hasBones)
      {
        ///////////////////////////////////////////////////////
        // Don't have time to test, pretty sure this should be here though, delete the above.
        ///////////////////////////////////////////////////////
        //pMeshScene = ImporterCol.ReadFile(meshFile.c_str(),
        //  aiProcess_Triangulate |
        //  aiProcess_PreTransformVertices |
        //  aiProcess_CalcTangentSpace |
        //  aiProcess_GenSmoothNormals);































        pMeshScene = pColliderScene;
      }

      mParts.clear();
      mParts.reserve(pMeshScene->mNumMeshes);

      //printf("Mesh FileName: %s\n", aFile.c_str());

      uint32_t numMeshes = pMeshScene->mNumMeshes;

      // Load Mesh
      uint32_t startingVertex = 0;
      for (u32 i = 0; i < numMeshes; i++)
      {
        mParts.emplace_back(aRenderer,
                            this,
                            pMeshScene, 
                            pMeshScene->mMeshes[i], 
                            &mSkeleton, 
                            startingVertex);
        startingVertex += pMeshScene->mMeshes[i]->mNumVertices;
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

  void Mesh::UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices)
  {
    DebugObjection(
      aVertices.size() != mParts[aSubmeshIndex].mData.mVertexData.size(), 
      "UpdateVertices cannot change the size of the vertex buffer from %i to %i", 
      mParts[aSubmeshIndex].mData.mVertexData.size(), 
      aVertices.size());

    mParts[aSubmeshIndex].mData.mVertexData = aVertices;

    mDimension = CalculateDimensions(mParts);
  }

  void Mesh::UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices)
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
    //auto range = ContiguousRange<Submesh>(mParts);
    //auto range2 = ContiguousRange<Submesh>(mParts[0]);
    //
    //std::array<Submesh, 5> submeshes;
    //
    //auto range3 = MakeContiguousRange(submeshes);

    for (auto& submesh : mParts)
    {
      submesh.ResetTextureCoordinates();
    }
  }
}
