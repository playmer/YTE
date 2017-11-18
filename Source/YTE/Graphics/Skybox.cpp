#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Skybox.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / L"Textures/Originals";

    std::vector<std::string> result;

    for (auto & p : filesystem::directory_iterator(finalPath))
    {
      std::string str = p.path().filename().generic_string();

      result.push_back(str);
    }

    return result;
  }


  YTEDefineType(Skybox)
  {
    YTERegisterType(Skybox);

    YTEBindProperty(&Skybox::GetTexture, &Skybox::SetTexture, "Texture")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);
  }

  Skybox::Skybox(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    //auto renderer = aSpace->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    //auto window = aSpace->GetComponent<GraphicsView>()->GetWindow();

    mTextureName = "skybox_cubemap.png";

    DeserializeByType<Skybox*>(aProperties, this, Skybox::GetStaticType());
  }

  Skybox::~Skybox()
  {

  }

  void Skybox::Initialize()
  {
    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    mTransform = mOwner->GetComponent<Transform>();

    mOwner->YTERegister(Events::PositionChanged, this, &Skybox::TransformUpdate);
    mOwner->YTERegister(Events::RotationChanged, this, &Skybox::TransformUpdate);
    mOwner->YTERegister(Events::ScaleChanged, this, &Skybox::TransformUpdate);

    CreateSkybox();
  }


  void Skybox::TransformUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    CreateTransform();

    if (mInstantiatedSkybox)
    {
      mInstantiatedSkybox->UpdateUBOModel(mUBOModel);
    }
  }

  void Skybox::CreateTransform()
  {
    if (mTransform == nullptr)
    {
      return;
    }

    mUBOModel.mModelMatrix = glm::translate(glm::mat4(1.0f), mTransform->GetTranslation());

    mUBOModel.mModelMatrix = mUBOModel.mModelMatrix * glm::toMat4(mTransform->GetRotation());

    mUBOModel.mModelMatrix = glm::scale(mUBOModel.mModelMatrix, mTransform->GetScale());
  }

  void Skybox::CreateSkybox()
  {
    if (nullptr != mInstantiatedSkybox)
    {
      mInstantiatedSkybox.reset();
    }

    std::string meshName = "__SkyBox";
    meshName += mTextureName;

    Submesh submesh;

    Vertex vert0;
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;
    Vertex vert4;
    Vertex vert5;
    Vertex vert6;
    Vertex vert7;

    vert0.mPosition = glm::vec3(-0.5, -0.5, 0.5);
    vert1.mPosition = glm::vec3(0.5, -0.5, 0.5);
    vert2.mPosition = glm::vec3(0.5, 0.5, 0.5);
    vert3.mPosition = glm::vec3(-0.5, 0.5, 0.5);

    vert4.mPosition = glm::vec3(-0.5, -0.5, -0.5);
    vert5.mPosition = glm::vec3(0.5, -0.5, -0.5);
    vert6.mPosition = glm::vec3(0.5, 0.5, -0.5);
    vert7.mPosition = glm::vec3(-0.5, 0.5, -0.5);

    //std::vector<u32> mIndices{
    //  0, 1, 2,
    //  2, 3, 0,
    //
    //  1, 5, 6,
    //  6, 2, 1,
    //
    //  4, 0, 3,
    //  3, 7, 4,
    //
    //  5, 4, 7,
    //  7, 6, 5,
    //
    //  3, 2, 6,
    //  6, 7, 3,
    //
    //  4, 5, 1,
    //  1, 0, 4
    //};

    std::vector<u32> mIndices{
      2, 1, 0,
      0, 3, 2,

      6, 5, 1,
      1, 2, 6,
      
      3, 0, 4,
      4, 7, 3,
      
      7, 4, 5,
      5, 6, 7,
      
      6, 2, 3,
      3, 7, 6,
      
      1, 5, 4,
      4, 0, 1
    };

    submesh.mDiffuseMap = mTextureName;
    submesh.mDiffuseType = TextureViewType::eCube;
    submesh.mShaderSetName = "Skybox";

    submesh.mVertexBuffer.emplace_back(vert0);
    submesh.mVertexBuffer.emplace_back(vert1);
    submesh.mVertexBuffer.emplace_back(vert2);
    submesh.mVertexBuffer.emplace_back(vert3);
    submesh.mVertexBuffer.emplace_back(vert4);
    submesh.mVertexBuffer.emplace_back(vert5);
    submesh.mVertexBuffer.emplace_back(vert6);
    submesh.mVertexBuffer.emplace_back(vert7);

    submesh.mIndexBuffer = std::move(mIndices);

    submesh.mVertexBufferSize = submesh.mVertexBuffer.size() * sizeof(Vertex);
    submesh.mIndexBufferSize = submesh.mIndexBuffer.size() * sizeof(u32);

    std::vector<Submesh> submeshes{ submesh };

    auto mesh = mRenderer->CreateSimpleMesh(mWindow, meshName, submeshes);

    mInstantiatedSkybox = mRenderer->CreateModel(mWindow, mesh);
    CreateTransform();
    mInstantiatedSkybox->UpdateUBOModel(mUBOModel);
  }
}