#pragma once

#include "assimp/vector3.h"
#include "assimp/types.h"

#include "YTE/Core/PrivateImplementation.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/Renderer.hpp"
#include "YTE/Graphics/Vertex.hpp"

#include "YTE/Platform/ForwardDeclarations.h"

struct aiScene;
struct aiMesh;

namespace YTE
{
  struct UBOModel
  {
    glm::mat4 mModelMatrix;
  };

  struct UBOMaterial
  {
    glm::vec3 mDiffuse;
    glm::vec3 mAmbient;
    glm::vec3 mSpecular;
    glm::vec3 mEmissive;
    glm::vec3 mTransparent;
    glm::vec3 mReflective;
    float mOpacity = 1.0f;
    float mShininess = 1.0f;
    float mShininessStrength = 1.0f;
    float mReflectivity = 1.0f;
    float mReflectiveIndex = 1.0f;
    float mBumpScaling = 1.0f;
  };

  inline glm::vec3 AssimpToGLM(const aiVector3D *aVector)
  {
    return { aVector->x, aVector->y ,aVector->z };
  }

  inline glm::vec3 AssimpToGLM(const aiColor3D *aVector)
  {
    return { aVector->r, aVector->g ,aVector->b };
  }


  struct Mesh
  {
    /** @brief Stores vertex and index base and counts for each part of a model */
    struct SubMesh
    {
      struct Dimension
      {
        glm::vec3 mMin = glm::vec3(FLT_MAX);
        glm::vec3 mMax = glm::vec3(-FLT_MAX);
        glm::vec3 mSize;
      };

      SubMesh(Renderer *aRenderer,
              Window *aWindow,
              const aiScene *aScene,
              const aiMesh *aMesh);

      SubMesh(SubMesh &&aRight);

      std::vector<Vertex> mVertexBuffer;
      std::vector<u32> mIndexBuffer;

      UBOMaterial mUBOMaterial;

      Texture *mDiffuseMap;
      Texture *mNormalMap;
      Texture *mSpecularMap;

      Dimension mDimension;

      size_t mVertexBufferSize = 0;
      size_t mIndexBufferSize = 0;

      std::string mName;
      std::string mMaterialName;
      std::string mShaderSetName;

      PrivateImplementationDynamic mRendererData;
    };

    /** @brief Vertex layout components */
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

    struct CreateInfo
    {
      glm::vec3 mScale;
      glm::vec2 mUVscale;
      glm::vec3 mCenter;
    };

    Mesh(Renderer *aRenderer, 
         Window *aWindow, 
         std::string &aFile, 
         CreateInfo *aCreateInfo = nullptr);

    ~Mesh();

    std::vector<SubMesh> mParts;
  };
}
