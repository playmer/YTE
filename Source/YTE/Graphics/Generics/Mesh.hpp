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



    Submesh(Window *aWindow,
            const aiScene *aScene,
            const aiMesh *aMesh);

    virtual ~Submesh()
    {
      
    }



    std::vector<Vertex> mVertexBuffer;
    std::vector<u32> mIndexBuffer;

    UBOMaterial mUBOMaterial;

    std::string mDiffuseMap;
    std::string mNormalMap;
    std::string mSpecularMap;

    Dimension mDimension;

    size_t mVertexBufferSize = 0;
    size_t mIndexBufferSize = 0;

    std::string mName;
    std::string mMaterialName;
    std::string mShaderSetName;
  };



  class Mesh : public EventHandler
  {
  public:
    YTEDeclareType(Mesh);

    Mesh(Window *aWindow,
         std::string &aFile,
         CreateInfo *aCreateInfo = nullptr);

    virtual ~Mesh();

    std::string mName;
    std::vector<Submesh> mParts;
  };
}

#endif