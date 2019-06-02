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

    submesh.mShaderSetName = mShaderSetName;

    submesh.mCullBackFaces = true;

    submesh.mVertexData = aVertices;
    submesh.mIndexData = aIndices;

    submesh.mTextureData.emplace_back(mDiffuseTName, TextureViewType::e2D, SubmeshData::TextureType::Diffuse);
    submesh.mTextureData.emplace_back(mSpecularTName, TextureViewType::e2D, SubmeshData::TextureType::Specular);
    submesh.mTextureData.emplace_back(mNormalTName, TextureViewType::e2D, SubmeshData::TextureType::Normal);

    auto mesh = mRenderer->CreateSimpleMesh(meshName, submesh);
    mModel = mRenderer->CreateModel(mGraphicsView, mesh);
  }
}