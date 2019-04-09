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

    for (auto const& vertex : mSubMesh.mVertexData)
    {
      mSubMesh.mDimension.mMax.x = fmax(vertex.mPosition.x, mSubMesh.mDimension.mMax.x);
      mSubMesh.mDimension.mMax.y = fmax(vertex.mPosition.y, mSubMesh.mDimension.mMax.y);
      mSubMesh.mDimension.mMax.z = fmax(vertex.mPosition.z, mSubMesh.mDimension.mMax.z);

      mSubMesh.mDimension.mMin.x = fmin(vertex.mPosition.x, mSubMesh.mDimension.mMin.x);
      mSubMesh.mDimension.mMin.y = fmin(vertex.mPosition.y, mSubMesh.mDimension.mMin.y);
      mSubMesh.mDimension.mMin.z = fmin(vertex.mPosition.z, mSubMesh.mDimension.mMin.z);
    }
  }

  Dimension CalculateDimensions(std::vector<Submesh> const& mParts)
  {
    YTEProfileFunction();
    Dimension toReturn;

    for (auto& part : mParts)
    {
      toReturn.mMax.x = fmax(part.mDimension.mMax.x, toReturn.mMax.x);
      toReturn.mMax.y = fmax(part.mDimension.mMax.y, toReturn.mMax.y);
      toReturn.mMax.z = fmax(part.mDimension.mMax.z, toReturn.mMax.z);
      toReturn.mMax.x = fmax(part.mDimension.mMin.x, toReturn.mMax.x);
      toReturn.mMax.y = fmax(part.mDimension.mMin.y, toReturn.mMax.y);
      toReturn.mMax.z = fmax(part.mDimension.mMin.z, toReturn.mMax.z);

      toReturn.mMin.x = fmin(part.mDimension.mMax.x, toReturn.mMin.x);
      toReturn.mMin.y = fmin(part.mDimension.mMax.y, toReturn.mMin.y);
      toReturn.mMin.z = fmin(part.mDimension.mMax.z, toReturn.mMin.z);
      toReturn.mMin.x = fmin(part.mDimension.mMin.x, toReturn.mMin.x);
      toReturn.mMin.y = fmin(part.mDimension.mMin.y, toReturn.mMin.y);
      toReturn.mMin.z = fmin(part.mDimension.mMin.z, toReturn.mMin.z);
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
                   const aiScene *aScene,
                   const aiMesh *aMesh,
                   Skeleton* aSkeleton,
                   uint32_t aBoneStartingVertexOffset)
  {
    YTEProfileFunction();

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

    mUBOMaterial.mDiffuse = glm::vec4(ToGlm(&Diffuse), 1.0f);
    mUBOMaterial.mAmbient = glm::vec4(ToGlm(&Ambient), 1.0f);
    mUBOMaterial.mSpecular = glm::vec4(ToGlm(&Specular), 1.0f);
    mUBOMaterial.mEmissive = glm::vec4(ToGlm(&Emissive), 1.0f);
    mUBOMaterial.mTransparent = glm::vec4(ToGlm(&Transparent), 1.0f);
    mUBOMaterial.mReflective = glm::vec4(ToGlm(&Reflective), 1.0f);
    mUBOMaterial.mFlags = 0;

    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_OPACITY, mUBOMaterial.mOpacity);
    material->Get(AI_MATKEY_SHININESS, mUBOMaterial.mShininess);
    material->Get(AI_MATKEY_SHININESS_STRENGTH, mUBOMaterial.mShininessStrength);
    material->Get(AI_MATKEY_REFLECTIVITY, mUBOMaterial.mReflectivity);
    material->Get(AI_MATKEY_REFRACTI, mUBOMaterial.mReflectiveIndex);
    material->Get(AI_MATKEY_BUMPSCALING, mUBOMaterial.mBumpScaling);

    mName = aMesh->mName.C_Str();
    mMaterialName = name.C_Str();

    // TODO (Andrew): Add ability to provide a shader if wanted
    //mShaderSetName = mMaterialName.substr(0, mMaterialName.find_first_of('_'));

    aiString diffuse;
    aiString specular;
    aiString normals;

    material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse);
    material->GetTexture(aiTextureType_SPECULAR, 0, &specular);
    material->GetTexture(aiTextureType_NORMALS, 0, &normals);

    std::string defaultTexture{ "white.png" };

    mDiffuseMap = defaultTexture;
    mSpecularMap = defaultTexture;
    mNormalMap = defaultTexture;

    if (0 != diffuse.length)
    {
      mDiffuseMap = diffuse.C_Str();
    }

    if (0 != specular.length)
    {
      mSpecularMap = specular.C_Str();
    }

    if (0 != normals.length)
    {
      mNormalMap = normals.C_Str();
      mUBOMaterial.mUsesNormalTexture = 1; // true
    }

    aRenderer->RequestTexture(mDiffuseMap);
    aRenderer->RequestTexture(mSpecularMap);
    aRenderer->RequestTexture(mNormalMap);

    mShaderSetName = "Phong";

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
      
      mVertexData.emplace_back(
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

      mInitialTextureCoordinates.emplace_back(textureCoordinates);
    }

    uint32_t indexBase = static_cast<uint32_t>(mIndexData.size());

    for (unsigned int j = 0; j < aMesh->mNumFaces; j++)
    {
      const aiFace &Face = aMesh->mFaces[j];

      if (Face.mNumIndices != 3)
      {
        continue;
      }

      mIndexData.push_back(indexBase + Face.mIndices[0]);
      mIndexData.push_back(indexBase + Face.mIndices[1]);
      mIndexData.push_back(indexBase + Face.mIndices[2]);
    }

    Initialize();
  }

  Submesh::Submesh(Submesh&& aRight)
    : mVertexData{ std::move(aRight.mVertexData) }
    , mIndexData{ std::move(aRight.mIndexData) }
    , mVertexBuffer{ std::move(aRight.mVertexBuffer) }
    , mIndexBuffer{ std::move(aRight.mIndexBuffer) }
    , mInitialTextureCoordinates{ std::move(aRight.mInitialTextureCoordinates) }
    , mUBOMaterial{ std::move(aRight.mUBOMaterial) }
    , mDiffuseMap{ std::move(aRight.mDiffuseMap) }
    , mDiffuseType{ std::move(aRight.mDiffuseType) }
    , mNormalMap{ std::move(aRight.mNormalMap) }
    , mNormalType{ std::move(aRight.mNormalType) }
    , mSpecularMap{ std::move(aRight.mSpecularMap) }
    , mSpecularType{ std::move(aRight.mSpecularType) }
    , mDimension{ std::move(aRight.mDimension) }
    , mName{ std::move(aRight.mName) }
    , mMaterialName{ std::move(aRight.mMaterialName) }
    , mShaderSetName{ std::move(aRight.mShaderSetName) }
    , mMesh{ std::move(aRight.mMesh) }
    , mCullBackFaces{ std::move(aRight.mCullBackFaces) }
  {

  }

  Submesh& Submesh::operator=(Submesh&& aRight)
  {
    mVertexData = std::move(aRight.mVertexData);
    mIndexData = std::move(aRight.mIndexData);
    mVertexBuffer = std::move(aRight.mVertexBuffer);
    mIndexBuffer = std::move(aRight.mIndexBuffer);
    mInitialTextureCoordinates = std::move(aRight.mInitialTextureCoordinates);
    mUBOMaterial = std::move(aRight.mUBOMaterial);
    mDiffuseMap = std::move(aRight.mDiffuseMap);
    mDiffuseType = std::move(aRight.mDiffuseType);
    mNormalMap = std::move(aRight.mNormalMap);
    mNormalType = std::move(aRight.mNormalType);
    mSpecularMap = std::move(aRight.mSpecularMap);
    mSpecularType = std::move(aRight.mSpecularType);
    mDimension = std::move(aRight.mDimension);
    mName = std::move(aRight.mName);
    mMaterialName = std::move(aRight.mMaterialName);
    mShaderSetName = std::move(aRight.mShaderSetName);
    mMesh = std::move(aRight.mMesh);
    mCullBackFaces = std::move(aRight.mCullBackFaces);
  }

  void Submesh::Initialize()
  {
    CalculateSubMeshDimensions(*this);
    CreateGPUBuffers();
  }

  void Submesh::CreateGPUBuffers()
  {
    auto allocator = mMesh->mRenderer->GetAllocator(AllocatorTypes::Mesh);

    // Create Vertex, Index, and Material buffers.
    mVertexBuffer = allocator->CreateBuffer<Vertex>(mVertexData.size(),
                                                    GPUAllocation::BufferUsage::TransferDst |
                                                    GPUAllocation::BufferUsage::VertexBuffer,
                                                    GPUAllocation::MemoryProperty::DeviceLocal);

    mIndexBuffer = allocator->CreateBuffer<u32>(mIndexData.size(),
                                                GPUAllocation::BufferUsage::TransferDst |
                                                GPUAllocation::BufferUsage::IndexBuffer,
                                                GPUAllocation::MemoryProperty::DeviceLocal);
  }

  void Submesh::ResetTextureCoordinates()
  {
    for (size_t i = 0; i < mVertexData.size(); i++)
    {
      mVertexData[i].mTextureCoordinates = mInitialTextureCoordinates[i];
    }
  }

  void Submesh::UpdateVertices(std::vector<Vertex>& aVertices)
  {
    DebugObjection(aVertices.size() != mVertexData.size(), 
                   "UpdateVerticesAndIndices cannot change the size of the vertex buffer from %i to %i", 
                   mVertexData.size(), 
                   aVertices.size());

    mVertexData = aVertices;

    mVertexBuffer.Update(mVertexData.data(), mVertexData.size());

    CalculateSubMeshDimensions(*this);
  }

  void Submesh::UpdateVerticesAndIndices(std::vector<Vertex>& aVertices, std::vector<u32>& aIndices)
  {
    DebugObjection(aVertices.size() != mVertexData.size(), 
                   "UpdateVerticesAndIndices cannot change the size of the vertex buffer from %i to %i", 
                   mVertexData.size(), 
                   aVertices.size());

    DebugObjection(aIndices.size() != mIndexData.size(), 
                   "UpdateVerticesAndIndices cannot change the size of the index buffer from %i to %i", 
                   mIndexData.size(), 
                   aIndices.size());

    mVertexData = aVertices;
    mIndexData = aIndices;

    mVertexBuffer.Update(mVertexData.data(), mVertexData.size());
    mIndexBuffer.Update(mIndexData.data(), mIndexData.size());

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
    : mInstanced(false)
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

      // extra setup
      glm::vec3 scale(1.0f);
      glm::vec2 uvscale(1.0f);
      glm::vec3 center(0.0f);

      auto pMeshScene = pScene;
      if (!hasBones)
      {
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
                            pMeshScene, 
                            pMeshScene->mMeshes[i], 
                            &mSkeleton, 
                            startingVertex);
        mParts[i].mMesh = this;
        startingVertex += pMeshScene->mMeshes[i]->mNumVertices;
      }

      mName = aFile;
    }

    mDimension = CalculateDimensions(mParts);
  }

  Mesh::Mesh(const std::string &aFile,
             std::vector<Submesh> &aSubmeshes)
    : mInstanced(false)
  {
    YTEProfileFunction();
    mName = aFile;
    mParts = aSubmeshes;

    for (auto& submesh : mParts)
    {
      submesh.mMesh = this;
      CalculateSubMeshDimensions(submesh);
    }

    mDimension = CalculateDimensions(mParts);
  }

  Mesh::Mesh(const std::string& aFile, Submesh& aSubmesh)
  {
    YTEProfileFunction();
    mName = aFile;
    mParts.emplace_back(std::move(aSubmesh));

    auto& submesh = mParts[0];
    submesh.mMesh = this;

    CalculateSubMeshDimensions(submesh);

    mDimension = CalculateDimensions(mParts);
  }

  void Mesh::UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices)
  {
    DebugObjection(aVertices.size() != mParts[aSubmeshIndex].mVertexData.size(), "UpdateVertices cannot change the size of the vertex buffer from %i to %i", mParts[aSubmeshIndex].mVertexData.size(), aVertices.size());
    mParts[aSubmeshIndex].mVertexData = aVertices;

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
      sub.mCullBackFaces = aCulling;
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
