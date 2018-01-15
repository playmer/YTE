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
    , mConstructing(true)
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
    mConstructing = false;
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

  Submesh CreateSphere(u32 aSubdivisions, const std::string &aTextureName)
  {
    float subdivisions = static_cast<float>(aSubdivisions);
    Submesh sphere;
    
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
          
          sphere.mIndexBuffer.emplace_back(c);
          sphere.mIndexBuffer.emplace_back(b);
          sphere.mIndexBuffer.emplace_back(a);
          
          sphere.mIndexBuffer.emplace_back(a);
          sphere.mIndexBuffer.emplace_back(d);
          sphere.mIndexBuffer.emplace_back(c);
        }

        sphere.mVertexBuffer.push_back(vert);
      }
    }

    sphere.mVertexBufferSize = sphere.mVertexBuffer.size() * sizeof(Vertex);
    sphere.mIndexBufferSize = sphere.mIndexBuffer.size() * sizeof(u32);

    sphere.mDiffuseMap = aTextureName;
    sphere.mDiffuseType = TextureViewType::e2D;
    sphere.mShaderSetName = "Skybox";

    return sphere;
  }

  void Skybox::CreateSkybox()
  {
    if (nullptr != mInstantiatedSkybox)
    {
      mInstantiatedSkybox.reset();
    }

    std::string meshName = "__SkyBox";
    meshName += mTextureName;

    //Submesh submesh;
    //
    //Vertex vert0;
    //Vertex vert1;
    //Vertex vert2;
    //Vertex vert3;
    //Vertex vert4;
    //Vertex vert5;
    //Vertex vert6;
    //Vertex vert7;
    //
    //vert0.mPosition = glm::vec3(-1.0, -1.0, 1.0);
    //vert1.mPosition = glm::vec3(1.0, -1.0, 1.0);
    //vert2.mPosition = glm::vec3(1.0, 1.0, 1.0);
    //vert3.mPosition = glm::vec3(-1.0, 1.0, 1.0);
    //
    //vert4.mPosition = glm::vec3(-1.0, -1.0, -1.0);
    //vert5.mPosition = glm::vec3(1.0, -1.0, -1.0);
    //vert6.mPosition = glm::vec3(1.0, 1.0, -1.0);
    //vert7.mPosition = glm::vec3(-1.0, 1.0, -1.0);
    //
    //std::vector<u32> mIndices{
    //  2, 1, 0,
    //  0, 3, 2,
    //
    //  6, 5, 1,
    //  1, 2, 6,
    //  
    //  3, 0, 4,
    //  4, 7, 3,
    //  
    //  7, 4, 5,
    //  5, 6, 7,
    //  
    //  6, 2, 3,
    //  3, 7, 6,
    //  
    //  1, 5, 4,
    //  4, 0, 1
    //};
    //
    //submesh.mDiffuseMap = mTextureName;
    //submesh.mDiffuseType = TextureViewType::eCube;
    //submesh.mShaderSetName = "Skybox";
    //
    //submesh.mVertexBuffer.emplace_back(vert0);
    //submesh.mVertexBuffer.emplace_back(vert1);
    //submesh.mVertexBuffer.emplace_back(vert2);
    //submesh.mVertexBuffer.emplace_back(vert3);
    //submesh.mVertexBuffer.emplace_back(vert4);
    //submesh.mVertexBuffer.emplace_back(vert5);
    //submesh.mVertexBuffer.emplace_back(vert6);
    //submesh.mVertexBuffer.emplace_back(vert7);
    //
    //submesh.mIndexBuffer = std::move(mIndices);
    //
    //submesh.mVertexBufferSize = submesh.mVertexBuffer.size() * sizeof(Vertex);
    //submesh.mIndexBufferSize = submesh.mIndexBuffer.size() * sizeof(u32);

    auto submesh = CreateSphere(128, mTextureName);

    std::vector<Submesh> submeshes{ submesh };

    auto view = mSpace->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(view, meshName, submeshes);

    mInstantiatedSkybox = mRenderer->CreateModel(view, mesh);
    CreateTransform();
    mInstantiatedSkybox->UpdateUBOModel(mUBOModel);
  }
}
