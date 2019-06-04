#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Skybox.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.hpp"

#include "fmt/format.h"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    UnusedArguments(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / L"Textures/Originals";

    std::vector<std::string> result;

    result.push_back("None");

    for (auto & p : filesystem::directory_iterator(finalPath))
    {
      std::string str = p.path().filename().generic_string();

      result.push_back(str);
    }

    return result;
  }


  YTEDefineType(Skybox)
  {
    RegisterType<Skybox>();
    TypeBuilder<Skybox> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    builder.Property<&Skybox::GetTexture, &Skybox::SetTexture>("Texture")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);
  }

  Skybox::Skybox(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mTextureName("None")
    , mConstructing(true)
  {
  }

  Skybox::~Skybox()
  {

  }

  void Skybox::AssetInitialize()
  {
    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();

    if (mTextureName.empty())
    {
      return;
    }

    mRenderer->RequestTexture(mTextureName);
  }

  void Skybox::NativeInitialize()
  {
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    mTransform = mOwner->GetComponent<Transform>();

    mOwner->RegisterEvent<&Skybox::TransformUpdate>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&Skybox::TransformUpdate>(Events::RotationChanged, this);
    mOwner->RegisterEvent<&Skybox::TransformUpdate>(Events::ScaleChanged, this);

    CreateSkybox();
    mConstructing = false;
  }


  void Skybox::TransformUpdate(TransformChanged *aEvent)
  {
    UnusedArguments(aEvent);

    CreateTransform();

    if (mInstantiatedSkybox)
    {
      mUBOModel.mDiffuseColor = mInstantiatedSkybox->GetUBOModelData().mDiffuseColor;
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

  SubmeshData CreateSphere(u32 aSubdivisions, const std::string &aTextureName)
  {
    float subdivisions = static_cast<float>(aSubdivisions);
    SubmeshData sphere;
    
    const float pi = glm::pi<float>();
    const float tau = 2.0f * pi;

    for (u32 i = 0;  i <= aSubdivisions * 2;  i++)
    {
      float s = i * tau / (subdivisions * 2.0f);
      for (u32 j = 0; j <= aSubdivisions; j++)
      {
        float t = j * pi / subdivisions;
        float x = cos(s) * sin(t);
        float y = sin(s) * sin(t);
        float z = cos(t);

        Vertex vert;
        vert.mPosition = glm::vec3{ x, y, z };
        vert.mNormal   = glm::vec3{ x, y, z };
        vert.mTextureCoordinates = glm::vec3{ s / (tau), t / pi, 1.0f };
        vert.mTangent = glm::vec3{ -sin(s), cos(s), 0.0f };

        if ((i > 0) && (j > 0))
        {
          u32 a{ (i - 1)*(aSubdivisions + 1) + (j - 1) };
          u32 b{ (i - 1)*(aSubdivisions + 1) + (j) };
          u32 c{ (i)*(aSubdivisions + 1) + (j) };
          u32 d{ (i)*(aSubdivisions + 1) + (j - 1) };
          
          sphere.mIndexData.emplace_back(c);
          sphere.mIndexData.emplace_back(b);
          sphere.mIndexData.emplace_back(a);
          
          sphere.mIndexData.emplace_back(a);
          sphere.mIndexData.emplace_back(d);
          sphere.mIndexData.emplace_back(c);
        }

        sphere.mVertexData.AddVertex(vert);
      }
    }

    sphere.mTextureData.emplace_back(aTextureName, TextureViewType::e2D, SubmeshData::TextureType::Diffuse);
    //sphere.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Specular);
    //sphere.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Normal);


    sphere.mShaderSetName = "Skybox";

    return std::move(sphere);
  }


  void Skybox::SetTexture(std::string &aTexture)
  {
    if (aTexture.empty() || aTexture == mTextureName)
    {
      return;
    }

    std::filesystem::path path(aTexture);
    std::string extension = path.extension().u8string();

    if (aTexture == "None")
    {
      mTextureName = aTexture;
      mInstantiatedSkybox.reset();
    }
    else if (extension != ".png")
    {
      return;
    }
    else
    {
      mTextureName = aTexture;

      if (mConstructing)
      {
        return;
      }

      mInstantiatedSkybox.reset();
      CreateSkybox();
    }
  }

  void Skybox::CreateSkybox()
  {
    if (nullptr != mInstantiatedSkybox)
    {
      mInstantiatedSkybox.reset();
    }

    if (mTextureName.empty() || "None" == mTextureName)
    {
      return;
    }

    //bool success = FileCheck(Path::GetGamePath(), "Textures/Originals", mTextureName);
    //
    //if (false == success)
    //{
    //  success = FileCheck(Path::GetEnginePath(), "Textures/Originals", mTextureName);
    //}
    //
    //if (false == success)
    //{
    //  auto log = fmt::format("Skybox ({}): Texture of name {} is not found.", 
    //                         mOwner->GetName().c_str(), 
    //                         mTextureName);
    //
    //  mOwner->GetEngine()->Log(LogType::Warning, log);
    //  return;
    //}

    std::string meshName = "__SkyBox";
    meshName += mTextureName;

    auto submesh = CreateSphere(8, mTextureName);

    auto view = mSpace->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(meshName, submesh);

    mInstantiatedSkybox = mRenderer->CreateModel(view, mesh);
    CreateTransform();
    mUBOModel.mDiffuseColor = mInstantiatedSkybox->GetUBOModelData().mDiffuseColor;
    mInstantiatedSkybox->UpdateUBOModel(mUBOModel);
  }
}
