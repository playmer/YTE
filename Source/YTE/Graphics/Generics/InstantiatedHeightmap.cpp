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
    YTERegisterType(InstantiatedHeightmap);
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

    CreateMesh(aVertices, aIndices, aModelName, {}, {}, {});
  }

  void InstantiatedHeightmap::Initialize(std::string& aModelName,
                                         std::vector<Vertex>& aVertices,
                                         std::vector<u32>& aIndices,
                                         std::string& aShaderSetName,
                                         GraphicsView* aView,
                                         Renderer* aRenderer,
                                         std::vector<std::pair<vkhlf::Sampler*, vkhlf::ImageView*>> fbs,
                                         std::vector<TextureInformation> texs,
                                         std::vector<UBOInformation> ubos)
  {
    mShaderSetName = aShaderSetName;
    mGraphicsView = aView;
    mRenderer = aRenderer;

    CreateMesh(aVertices, aIndices, aModelName, fbs, texs, ubos);
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


  void InstantiatedHeightmap::CreateMesh(std::vector<Vertex>& aVertices,
                                         std::vector<u32>& aIndices,
                                         std::string& aModelName,
                                         std::vector<std::pair<vkhlf::Sampler*, vkhlf::ImageView*>> fbs,
                                         std::vector<TextureInformation> texs,
                                         std::vector<UBOInformation> ubos)
  {
    std::string meshName = aModelName;
    Submesh submesh;

    submesh.mShaderSetName = mShaderSetName;

    submesh.mCullBackFaces = true;

    submesh.mVertexBuffer = aVertices;
    submesh.mIndexBuffer = aIndices;

    submesh.mVertexBufferSize = submesh.mVertexBuffer.size() * sizeof(Vertex);
    submesh.mIndexBufferSize = submesh.mIndexBuffer.size() * sizeof(u32);

    submesh.mTextures.clear();
    for (size_t i = 0; i < texs.size(); ++i)
    {
      submesh.mTextures.push_back(texs[i]);
    }

    ShaderUsage useVert(true, false);
    ShaderUsage useFrag(false, true);
    submesh.mUBOs.clear();
    submesh.mUBOs.emplace_back(UBOTypeIDs::View, useVert, sizeof(UBOView));
    submesh.mUBOs.emplace_back(UBOTypeIDs::Animation, useVert, sizeof(UBOAnimation));
    submesh.mUBOs.emplace_back(UBOTypeIDs::ModelMaterial, useFrag, sizeof(UBOMaterial));
    submesh.mUBOs.emplace_back(UBOTypeIDs::SubmeshMaterial, useFrag, sizeof(UBOMaterial));
    submesh.mUBOs.emplace_back(UBOTypeIDs::Lights, useFrag, sizeof(UBOLightMan));
    submesh.mUBOs.emplace_back(UBOTypeIDs::Illumination, useFrag, sizeof(UBOIllumination));
    submesh.mUBOs.emplace_back(UBOTypeIDs::Model, useVert, sizeof(UBOModel));
    submesh.mUBOs.emplace_back(UBOTypeIDs::ClipPlanes, useFrag, sizeof(UBOClipPlanes));
    for (size_t i = 0; i < ubos.size(); ++i)
    {
      submesh.mUBOs.push_back(ubos[i]);
    }

    std::vector<Submesh> submeshes{ submesh };
    Mesh* mesh = mRenderer->CreateSimpleMesh(meshName, submeshes);
    mModel = mRenderer->CreateModel(mGraphicsView, mesh);
  }
}