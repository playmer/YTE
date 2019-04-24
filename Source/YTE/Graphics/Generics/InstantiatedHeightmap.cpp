///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Generics/InstantiatedHeightmap.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedHeightmap)
  {
    RegisterType<InstantiatedHeightmap>();
    TypeBuilder<InstantiatedHeightmap> builder;
  }



  InstantiatedHeightmap::InstantiatedHeightmap()
  {

  }

  InstantiatedHeightmap::~InstantiatedHeightmap()
  {
    mRenderer->DestroyMeshAndModel(mGraphicsView, mModel.get());
    mModel.reset(nullptr);
  }


  void InstantiatedHeightmap::Initialize(std::string& aModelName,
                                         std::vector<Vertex>& aVertices,
                                         std::vector<u32>& aIndices,
                                         std::string& aShaderSetName,
                                         GraphicsView* aView,
                                         Renderer* aRenderer)
  {
    mShaderSetName = aShaderSetName;
    mGraphicsView = aView;
    mRenderer = aRenderer;

    CreateMesh(aVertices, aIndices, aModelName);
  }

  void InstantiatedHeightmap::Initialize(std::string& aModelName,
                                         std::vector<Vertex>& aVertices,
                                         std::vector<u32>& aIndices,
                                         std::string& aShaderSetName,
                                         GraphicsView* aView,
                                         Renderer* aRenderer,
                                         std::string aDiffuseTextureName,
                                         std::string aSpecularTextureName,
                                         std::string aNormalTextureName)
  {
    mShaderSetName = aShaderSetName;
    mGraphicsView = aView;
    mRenderer = aRenderer;
    mDiffuseTName = aDiffuseTextureName;
    mSpecularTName = aSpecularTextureName;
    mNormalTName = aNormalTextureName;

    CreateMesh(aVertices, aIndices, aModelName);
  }

  void InstantiatedHeightmap::CreateMesh(std::vector<Vertex>& aVertices, std::vector<u32>& aIndices, std::string& aModelName)
  {
    std::string meshName = aModelName;
    SubmeshData submesh;

    submesh.mUBOMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    submesh.mUBOMaterial.mSpecular = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    submesh.mUBOMaterial.mShininess = 200.0f;

    //submesh.mDiffuseMap = mTextureName;
    submesh.mDiffuseType = TextureViewType::e2D;
    submesh.mShaderSetName = mShaderSetName;

    submesh.mCullBackFaces = true;

    submesh.mVertexData = aVertices;
    submesh.mIndexData = aIndices;

    submesh.mDiffuseMap = mDiffuseTName;
    submesh.mSpecularMap = mSpecularTName;
    submesh.mNormalMap = mNormalTName;

    auto mesh = mRenderer->CreateSimpleMesh(meshName, submesh);
    mModel = mRenderer->CreateModel(mGraphicsView, mesh);
  }
}