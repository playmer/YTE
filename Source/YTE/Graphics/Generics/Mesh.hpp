#pragma once

#ifndef YTE_Graphics_Generics_Mesh_hpp
#define YTE_Graphics_Generics_Mesh_hpp

#include <limits> 

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/Generics/Shader.hpp"

#include "YTE/Graphics/GPUBuffer.hpp"
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
  class Mesh;

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


    static YTE_Shared bool HasBones(const aiScene* aScene);

    YTE_Shared void Initialize(const aiScene* aScene);

    YTE_Shared void LoadBoneData(const aiMesh* aMesh, uint32_t aVertexStartingIndex);

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

    UBOs::Animation* GetDefaultOffsets()
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
    YTE_Shared void PreTransform(const aiScene* aScene);
    YTE_Shared void VisitNodes(const aiNode* aNode, glm::mat4 const& aParentTransform);

    std::map<std::string, uint32_t, std::less<>> mBones;
    std::vector<BoneData> mBoneData;
    uint32_t mNumBones;
    glm::mat4 mGlobalInverseTransform;
    std::vector<VertexSkeletonData> mVertexSkeletonData;
    UBOs::Animation mDefaultOffsets;
    //#ifdef _DEBUG
    //    std::vector<unsigned int> mVertexErrorAdds;
    //#endif.
  };

  // Dimension struct is used for bounding box of 3D mesh
  struct Dimension
  {
    glm::vec3 mMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 mMax = glm::vec3(-std::numeric_limits<float>::max());

    float GetRadius()
    {
      return std::max(glm::length(mMin), glm::length(mMax));
    }

    glm::vec3 GetCenter()
    {
      auto difference = mMax - mMin;

      return mMin + (difference * .5f);
    }
  };

  struct SubmeshData
  {
    enum class TextureType
    {
      Diffuse,
      Specular,
      Normal,
      Ambient,
      Emissive,
      Height,
      Shininess,
      Opacity,
      Displacment,
      Lightmap,
      Reflection,
      Unknown
    };

    struct TextureData
    {
      TextureData(std::string aName, TextureViewType aViewType, TextureType aSamplerType)
        : mName{ aName }
        , mViewType{ aViewType }
        , mSamplerType{ aSamplerType }
      {

      }

      std::string mName;
      TextureViewType mViewType = TextureViewType::e2D;
      TextureType mSamplerType;
    };

    ShaderDescriptions mDescriptions;
    std::vector<Vertex> mVertexData;
    std::vector<u32> mIndexData;
    std::vector<TextureData> mTextureData;

    std::vector<glm::vec3> mInitialTextureCoordinates;

    UBOs::Material mUBOMaterial;

    //std::string mDiffuseMap;
    //TextureViewType mDiffuseType = TextureViewType::e2D;
    //std::string mNormalMap;
    //TextureViewType mNormalType = TextureViewType::e2D;
    //std::string mSpecularMap;
    //TextureViewType mSpecularType = TextureViewType::e2D;

    Dimension mDimension;

    std::string mName;
    std::string mMaterialName;
    std::string mShaderSetName;
    Mesh* mMesh;
    bool mCullBackFaces = true;
  };

  // Submesh class contains all the data of the actual submesh
  class Submesh
  {
  public:
    YTE_Shared Submesh() = default;

    YTE_Shared Submesh(Renderer* aRenderer,
                       Mesh* aYTEMesh,
                       aiScene const* aScene,
                       aiMesh const* aMesh,
                       Skeleton* aSkeleton,
                       uint32_t aBoneStartingVertexOffset);

    YTE_Shared Submesh(SubmeshData&& aSubmesh);
    YTE_Shared Submesh(Submesh&& aSubmesh);
    YTE_Shared Submesh& operator=(Submesh&& aSubmesh);

    // Call this if you've built up a default constructed Submesh. 
    // This will create the GPU buffers and calculate the dimensions.
    void Initialize();

    size_t GetTriangleCount() const
    {
      return mData.mIndexData.size() / 3;
    }

    glm::uvec3 GetTriangle(size_t aIndex) const
    {
      glm::uvec3 tri;
      tri.x = (glm::uint)mData.mIndexData[aIndex];
      tri.y = (glm::uint)mData.mIndexData[aIndex + 1];
      tri.z = (glm::uint)mData.mIndexData[aIndex + 2];
      return tri;
    }

    ShaderDescriptions const& CreateShaderDescriptions();

    void ResetTextureCoordinates();

    // You probably shouldn't call these functions as it won't recalculate the owning mesh's dimensions
    void UpdateVertices(std::vector<Vertex> const& aVertices);
    void UpdateVerticesAndIndices(std::vector<Vertex> const& aVertices, std::vector<u32> const& aIndices);
    
    GPUBuffer<Vertex> mVertexBuffer;
    GPUBuffer<u32> mIndexBuffer;

    SubmeshData mData;

  private:
    void CreateGPUBuffers();

    YTE_Shared Submesh& operator=(Submesh const& aSubmesh) = delete;
    YTE_Shared Submesh(Submesh const& aSubmesh) = delete;
  };

  class Mesh : public EventHandler
  {
  public:
    YTEDeclareType(Mesh);
    
    YTE_Shared Mesh();

    YTE_Shared Mesh(Renderer *aRenderer,
                    const std::string &aFile);

    YTE_Shared Mesh(Renderer* aRenderer, 
                    const std::string &aFile,
                    ContiguousRange<SubmeshData> aSubmeshes);

    YTE_Shared virtual void UpdateVertices(size_t aSubmeshIndex, 
                                           std::vector<Vertex> const& aVertices);
    YTE_Shared virtual void UpdateVerticesAndIndices(size_t aSubmeshIndex, 
                                                     std::vector<Vertex> const& aVertices, 
                                                     std::vector<u32> const& aIndices);

    YTE_Shared virtual ~Mesh();

    YTE_Shared bool CanAnimate();
    YTE_Shared std::vector<Submesh>& GetSubmeshes();

    YTE_Shared void SetBackfaceCulling(bool aCulling);
    YTE_Shared virtual void RecreateShader() {}

    YTE_Shared void ResetTextureCoordinates();

    std::string mName;
    std::vector<Submesh> mParts;
    Renderer* mRenderer;
    Skeleton mSkeleton;
    Dimension mDimension;
    bool mInstanced;

  private:
    Mesh(Mesh const&) = delete;
    Mesh& operator=(Mesh const&) = delete;
  };
}

#endif
