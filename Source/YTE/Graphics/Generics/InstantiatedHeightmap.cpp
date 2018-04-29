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


  void InstantiatedHeightmap::UpdateMesh(std::vector<Vertex>& aVertices)
  {
    // Update the model
    auto mesh = mModel->GetMesh();
    mesh->UpdateVertices(0, aVertices);
  }


  void InstantiatedHeightmap::UpdateMesh(std::vector<Vertex>& aVertices, std::vector<u32>& aIndices)
  {
    // update the model
    auto mesh = mModel->GetMesh();
    mesh->UpdateVerticesAndIndices(0, aVertices, aIndices);
  }


  void InstantiatedHeightmap::CreateMesh(std::vector<Vertex>& aVertices, std::vector<u32>& aIndices, std::string& aModelName)
  {
    std::string meshName = aModelName;
    Submesh submesh;

    submesh.mUBOMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    submesh.mUBOMaterial.mSpecular = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    submesh.mUBOMaterial.mShininess = 200.0f;

    //submesh.mDiffuseMap = mTextureName;
    submesh.mDiffuseType = TextureViewType::e2D;
    submesh.mShaderSetName = mShaderSetName;

    submesh.mCullBackFaces = true;

    submesh.mVertexBuffer = aVertices;
    submesh.mIndexBuffer = aIndices;

    submesh.mDiffuseMap = mDiffuseTName;
    submesh.mSpecularMap = mSpecularTName;
    submesh.mNormalMap = mNormalTName;

    std::vector<Submesh> submeshes{ submesh };
    auto mesh = mRenderer->CreateSimpleMesh(meshName, submeshes);
    mModel = mRenderer->CreateModel(mGraphicsView, mesh);
  }
}