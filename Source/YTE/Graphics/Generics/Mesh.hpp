///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_Mesh_hpp
#define YTE_Graphics_Generics_Mesh_hpp

#include <limits> 

#include "assimp/types.h"
#include "assimp/vector3.h"

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vertex.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

// forward declarations for assimp
struct aiScene;
struct aiMesh;
struct aiNode;



namespace YTE
{
  // vertices have the following components in them
  enum class VertexComponent
  {
    VERTEX_COMPONENT_POSITION = 0x0,
    VERTEX_COMPONENT_NORMAL = 0x1,
    VERTEX_COMPONENT_COLOR = 0x2,
    VERTEX_COMPONENT_UV = 0x3,
    VERTEX_COMPONENT_TANGENT = 0x4,
    VERTEX_COMPONENT_BITANGENT = 0x5,
    VERTEX_COMPONENT_DUMMY_FLOAT = 0x6,
    VERTEX_COMPONENT_DUMMY_VEC4 = 0x7
  };

  // create info struct that can be passed into the mesh ctor
  struct CreateInfo
  {
    glm::vec3 mScale;
    glm::vec2 mUVscale;
    glm::vec3 mCenter;
  };


  enum class TextureViewType
  {
    e2D,
    eCube
  };


  struct BoneData
  {
    BoneData()
    {
      mOffset = glm::mat4{};
      mFinalTransformation = glm::mat4{};
    }

    BoneData(glm::mat4 const& aOffset) : mOffset(aOffset)
    {
      mFinalTransformation = glm::mat4{};
    }

    glm::mat4 mOffset;
    glm::mat4 mFinalTransformation;
  };


  // contains bones
  class Skeleton
  {
  public:
    // holds how a vertex and a skeleton match up
    struct VertexSkeletonData
    {
      std::array<uint32_t, BoneConstants::MaxBonesPerVertex> mIDs;
      std::array<float, BoneConstants::MaxBonesPerVertex> mWeights;

      VertexSkeletonData()
      {
        for (size_t i = 0; i < BoneConstants::MaxBonesPerVertex; ++i)
        {
          mIDs[i] = 0;
          mWeights[i] = 0.0f;
        }
      }

      bool AddBone(uint32_t aID, float aWeight)
      {
        for (uint32_t i = 0; i < BoneConstants::MaxBonesPerVertex; ++i)
        {
          if (mWeights[i] == 0.0f)
          {
            mIDs[i] = aID;
            mWeights[i] = aWeight;
            return true;  // bone added
          }
        }

        return false; // bone not added, too many bones on vertex
      }
    };

    bool Initialize(const aiScene* aScene);

    void LoadBoneData(const aiMesh* aMesh, uint32_t aVertexStartingIndex);

    bool HasBones()
    {
      return !mBones.empty();
    }

    std::vector<BoneData>& GetBoneData()
    {
      return mBoneData;
    }

    std::vector<VertexSkeletonData> const& GetVertexBoneData()
    {
      return mVertexSkeletonData;
    }

    UBOAnimation* GetDefaultOffsets()
    {
      return &mDefaultOffsets;
    }

    glm::mat4& GetGlobalInverseTransform()
    {
      return mGlobalInverseTransform;
    }

    std::map<std::string, uint32_t, std::less<>>* GetBones()
    {
      return &mBones;
    }

  private:
    void PreTransform(const aiScene* aScene);
    void VisitNodes(const aiNode* aNode, glm::mat4 const& aParentTransform);

    std::map<std::string, uint32_t, std::less<>> mBones;
    std::vector<BoneData> mBoneData;
    uint32_t mNumBones;
    glm::mat4 mGlobalInverseTransform;
    std::vector<VertexSkeletonData> mVertexSkeletonData;
    UBOAnimation mDefaultOffsets;
#ifdef _DEBUG
    std::vector<unsigned int> mVertexErrorAdds;
#endif
  };


  // Dimension struct is used for bounding box of 3D mesh
  struct Dimension
  {
    glm::vec3 mMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 mMax = glm::vec3(-std::numeric_limits<float>::max());
  };

  // Submesh class contains all the data of the actual submesh
  class Submesh
  {
  public:
    Submesh() = default;

    Submesh(Renderer *aRenderer,
            const aiScene *aScene,
            const aiMesh *aMesh,
            Skeleton *aSkeleton,
            uint32_t aBoneStartingVertexOffset);

    virtual ~Submesh()
    {
      
    }

    size_t GetTriangleCount()
    {
      return mIndexBuffer.size() / 3;
    }

    glm::uvec3 GetTriangle(size_t aIndex)
    {
      glm::uvec3 tri;
      tri.x = (glm::uint)mIndexBuffer[aIndex];
      tri.y = (glm::uint)mIndexBuffer[aIndex + 1];
      tri.z = (glm::uint)mIndexBuffer[aIndex + 2];
      return tri;
    }

    void ResetTextureCoordinates();

    std::vector<Vertex> mVertexBuffer;
    std::vector<u32> mIndexBuffer;

    std::vector<glm::vec3> mInitialTextureCoordinates;

    UBOMaterial mUBOMaterial;

    std::string mDiffuseMap;
    TextureViewType mDiffuseType = TextureViewType::e2D;
    std::string mNormalMap;
    TextureViewType mNormalType = TextureViewType::e2D;
    std::string mSpecularMap;
    TextureViewType mSpecularType = TextureViewType::e2D;

    Dimension mDimension;

    std::string mName;
    std::string mMaterialName;
    std::string mShaderSetName;
    bool mCullBackFaces = true;
  };


  struct ColliderMesh
  {
    ColliderMesh(const aiMesh* aMesh);
    std::vector<glm::vec3> mColliderVertexBuffer;
    std::vector<u32> mIndexBuffer;
  };


  class Mesh : public EventHandler
  {
  public:
    YTEDeclareType(Mesh);

    Mesh(Renderer *aRenderer,
         const std::string &aFile);

    Mesh(const std::string &aFile,
         std::vector<Submesh> &aSubmeshes);

    virtual void UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices);
    virtual void UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices);

    virtual ~Mesh();

    bool CanAnimate();
    std::vector<Submesh>& GetSubmeshes();

    void SetBackfaceCulling(bool aCulling);
    virtual void RecreateShader() {}

    void ResetTextureCoordinates();

    std::string mName;
    std::vector<Submesh> mParts;
    std::vector<ColliderMesh> mColliderParts;
    Skeleton mSkeleton;
    Dimension mDimension;
    bool mInstanced;

  private:
    void CreateCollider(const aiScene* aScene);
  };
}

#endif
