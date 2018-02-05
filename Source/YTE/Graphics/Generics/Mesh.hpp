///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_Mesh_hpp
#define YTE_Graphics_Generics_Mesh_hpp

#include "assimp/types.h"
#include "assimp/vector3.h"

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vertex.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

// forward declarations for assimp
struct aiScene;
struct aiMesh;
struct aiNode;



namespace YTE
{
  // inlined globals
  inline glm::vec3 AssimpToGLM(const aiVector3D *aVector)
  {
    return { aVector->x, aVector->y ,aVector->z };
  }

  inline glm::vec3 AssimpToGLM(const aiColor3D *aVector)
  {
    return { aVector->r, aVector->g ,aVector->b };
  }

  inline glm::quat AssimpToGLM(const aiQuaternion *aQuat) 
  { 
    glm::quat quaternion; 
 
    quaternion.x = aQuat->x; 
    quaternion.y = aQuat->y; 
    quaternion.z = aQuat->z; 
    quaternion.w = aQuat->w; 
 
    return quaternion; 
  } 
 
  inline glm::mat4 AssimpToGLM(const aiMatrix4x4 aMatrix) 
  { 
    glm::mat4 result; 
    for (int i = 0; i < 4; ++i) 
    { 
      for (int j = 0; j < 4; ++j) 
      { 
        result[i][j] = aMatrix[i][j]; 
      } 
    } 
 
    return result; 
  } 


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
      mOffset = aiMatrix4x4();
      mFinalTransformation = aiMatrix4x4();
    }

    BoneData(aiMatrix4x4 aOffset) : mOffset(aOffset)
    {
      mFinalTransformation = aiMatrix4x4();
    }

    aiMatrix4x4 mOffset;
    aiMatrix4x4 mFinalTransformation;
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

    aiMatrix4x4& GetGlobalInverseTransform()
    {
      return mGlobalInverseTransform;
    }

    std::unordered_map<std::string, uint32_t>* GetBones()
    {
      return &mBones;
    }


  private:
    void PreTransform(const aiScene* aScene);
    void VisitNodes(const aiNode* aNode, const aiMatrix4x4& aParentTransform);

    std::unordered_map<std::string, uint32_t> mBones;
    std::vector<BoneData> mBoneData;
    uint32_t mNumBones;
    aiMatrix4x4 mGlobalInverseTransform;
    std::vector<VertexSkeletonData> mVertexSkeletonData;
    UBOAnimation mDefaultOffsets;
#ifdef _DEBUG
    std::vector<unsigned int> mVertexErrorAdds;
#endif
  };



  // Submesh class contains all the data of the actual submesh
  class Submesh
  {
  public:
    // Dimension struct is used for bounding box of 3D mesh
    struct Dimension
    {
      glm::vec3 mMin = glm::vec3(FLT_MAX);
      glm::vec3 mMax = glm::vec3(-FLT_MAX);
      glm::vec3 mSize;
    };


    Submesh() = default;

    Submesh(Window *aWindow,
            const aiScene *aScene,
            const aiMesh *aMesh,
            Skeleton *aSkeleton,
            uint32_t aBoneStartingVertexOffset,
            bool aIsEditorObject);

    virtual ~Submesh()
    {
      
    }

    std::vector<Vertex> mVertexBuffer;
    std::vector<u32> mIndexBuffer;

    UBOMaterial mUBOMaterial;

    std::string mDiffuseMap;
    TextureViewType mDiffuseType = TextureViewType::e2D;
    std::string mNormalMap;
    TextureViewType mNormalType = TextureViewType::e2D;
    std::string mSpecularMap;
    TextureViewType mSpecularType = TextureViewType::e2D;

    Dimension mDimension;

    size_t mVertexBufferSize = 0;
    size_t mIndexBufferSize = 0;

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

    Mesh(Window *aWindow,
         std::string &aFile,
         CreateInfo *aCreateInfo = nullptr);

    Mesh(Window *aWindow,
         std::string &aFile,
         std::vector<Submesh> &aSubmeshes);

    virtual ~Mesh();

    bool CanAnimate();

    void SetBackfaceCulling(bool aCulling);
    virtual void RecreateShader() {}

    std::string mName;
    std::vector<Submesh> mParts;
    Skeleton mSkeleton;
    std::vector<ColliderMesh> mColliderParts;

  private:
    void CreateCollider(const aiScene* aScene);

  protected:
    bool mInstanced;
  };
}

#endif
