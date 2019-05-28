#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    UnusedArguments(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / "Textures";

    std::vector<std::string> result;

    for (auto & p : filesystem::directory_iterator(finalPath))
    {
      std::string str = p.path().filename().generic_string();

      result.push_back(str);
    }

    return result;
  }


  YTEDefineType(Sprite)
  {
    RegisterType<Sprite>();
    TypeBuilder<Sprite> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    builder.Property<&Sprite::GetTexture, &Sprite::SetTexture>("Texture")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    builder.Property<&Sprite::GetColumns, &Sprite::SetColumns>("Columns")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&Sprite::GetRows, &Sprite::SetRows>("Rows")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&Sprite::GetFrames, &Sprite::SetFrames>("Frames")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&Sprite::GetSpeed, &Sprite::SetSpeed>("Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("How many seconds it will take for the full animation to run.");

    builder.Property<&Sprite::GetAnimating, &Sprite::SetAnimating>("Animating")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  Sprite::Sprite(Composition *aOwner, Space *aSpace)
    : BaseModel{ aOwner, aSpace }
    , mConstructing{ true }
    , mAnimating{ false }
    , mColumns{ 1 }
    , mRows{ 1 }
    , mFrames{ 1 }
    , mSpeed{ 1.0f }
    , mTimeAccumulated{ 0.0 }
    , mCurrentIndex{ 0 }
  {
  }

  Sprite::~Sprite()
  {

  }

  void Sprite::AssetInitialize()
  {
    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();

    mConstructing = false;

    //mTextureName = "MENU_BUTTON_Yes.basis";

    if (mTextureName.empty())
    {
      return;
    }

    mRenderer->RequestTexture(mTextureName);
  }

  void Sprite::Initialize()
  {
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    mTransform = mOwner->GetComponent<Transform>();

    mOwner->RegisterEvent<&Sprite::TransformUpdate>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&Sprite::TransformUpdate>(Events::RotationChanged, this);
    mOwner->RegisterEvent<&Sprite::TransformUpdate>(Events::ScaleChanged, this);

    CreateSprite();
  }


  void Sprite::Update(LogicUpdate *aUpdate)
  {
    auto delta = mSpeed / mFrames;

    if (mInstantiatedSprite)
    {
      mTimeAccumulated += aUpdate->Dt;

      auto xWidth = 1.0f / mColumns;
      auto yHeight = 1.0f / mRows;

      if (mTimeAccumulated > delta)
      {
        mTimeAccumulated = 0.0;
        auto column = mCurrentIndex % mColumns;
        auto row = mCurrentIndex / mColumns;

        glm::vec3 uv0 = { 0.0f,   1.0f - yHeight, 0.0f }; // Bottom Left
        glm::vec3 uv1 = { xWidth, 1.0f - yHeight, 0.0f }; // Bottom Right
        glm::vec3 uv2 = { xWidth, 1.0f,           0.0f }; // Top Right
        glm::vec3 uv3 = { 0.0f,   1.0f,           0.0f }; // Top Left

        float xTranslate = xWidth * column;
        float yTranslate = -yHeight * row;

        uv0.x += xTranslate;
        uv0.y += yTranslate;
        uv1.x += xTranslate;
        uv1.y += yTranslate;
        uv2.x += xTranslate;
        uv2.y += yTranslate;
        uv3.x += xTranslate;
        uv3.y += yTranslate;

        // update the model
        auto mesh = mInstantiatedSprite->GetMesh();

        mVertexData[0].mTextureCoordinates = uv0;
        mVertexData[1].mTextureCoordinates = uv1;
        mVertexData[2].mTextureCoordinates = uv2;
        mVertexData[3].mTextureCoordinates = uv3;
        
        mesh->UpdateVertices(0, mVertexData);

        ++mCurrentIndex;

        if (mCurrentIndex > (mFrames - 1))
        {
          mCurrentIndex = 0;
        }
      }
    }
  }


  void Sprite::SetAnimating(bool aAnimating)
  {
    if (aAnimating == mAnimating)
    {
      return;
    }

    mAnimating = aAnimating;

    if (aAnimating)
    {
      mSpace->RegisterEvent<&Sprite::Update>(Events::LogicUpdate, this);
    }
    else
    {
      mSpace->DeregisterEvent<&Sprite::Update>(Events::LogicUpdate,  this);
    }
  }

  void Sprite::TransformUpdate(TransformChanged *aEvent)
  {
    UnusedArguments(aEvent);

    CreateTransform();

    if (mInstantiatedSprite)
    {
      mUBOModel.mDiffuseColor = mInstantiatedSprite->GetUBOModelData().mDiffuseColor;
      mInstantiatedSprite->UpdateUBOModel(mUBOModel);
    }
  }

  void Sprite::CreateTransform()
  {
    if (mTransform == nullptr)
    {
      return;
    }

    mUBOModel.mModelMatrix = glm::translate(glm::mat4(1.0f), mTransform->GetWorldTranslation());

    mUBOModel.mModelMatrix = mUBOModel.mModelMatrix * glm::toMat4(mTransform->GetWorldRotation());

    mUBOModel.mModelMatrix = glm::scale(mUBOModel.mModelMatrix, mTransform->GetWorldScale());
  }

  void Sprite::CreateSprite()
  {
    YTEProfileFunction();
    if (nullptr != mInstantiatedSprite)
    {
      mInstantiatedSprite.reset();
    }

    if (mTextureName.empty())
    {
      return;
    }

    std::string meshName = "__Sprite";
    meshName += mTextureName;
    meshName += mOwner->GetGUID().ToString();

    Vertex vert0;
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;

    vert0.mPosition = { -0.5, -0.5, 0.0 };
    vert0.mTextureCoordinates = { 0.0f, 0.0f, 0.0f };
    vert0.mNormal = { 0.0f, 0.0f, 1.0f };
    vert1.mPosition = { 0.5, -0.5, 0.0 };
    vert1.mTextureCoordinates = { 1.0f, 0.0f, 0.0f };
    vert1.mNormal = { 0.0f, 0.0f, 1.0f };
    vert2.mPosition = { 0.5, 0.5, 0.0 };
    vert2.mTextureCoordinates = { 1.0f, 1.0f, 0.0f };
    vert2.mNormal = { 0.0f, 0.0f, 1.0f };
    vert3.mPosition = { -0.5, 0.5, 0.0 };
    vert3.mTextureCoordinates = { 0.0f, 1.0f, 0.0f };
    vert3.mNormal = { 0.0f, 0.0f, 1.0f };

    if (mAnimating)
    {
      auto xWidth = 1.0f / mColumns;
      auto yHeight = 1.0f / mRows;

      vert0.mTextureCoordinates = { 0.0f,   1.0f - yHeight, 0.0f };
      vert1.mTextureCoordinates = { xWidth, 1.0f - yHeight, 0.0f };
      vert2.mTextureCoordinates = { xWidth, 1.0f,           0.0f };
      vert3.mTextureCoordinates = { 0.0f,   1.0f,           0.0f };
    }

    UBOs::Material modelMaterial{};
    modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mShininess = 1.0f;

    SubmeshData submesh;
    submesh.mUBOMaterial = modelMaterial;
    
    submesh.mDiffuseMap = mTextureName;
    submesh.mDiffuseType = TextureViewType::e2D;
    submesh.mShaderSetName = "Sprite";

    submesh.mCullBackFaces = false;

    submesh.mVertexData.clear();

    submesh.mVertexData.emplace_back(vert0);
    submesh.mVertexData.emplace_back(vert1);
    submesh.mVertexData.emplace_back(vert2);
    submesh.mVertexData.emplace_back(vert3);

    mVertexData = submesh.mVertexData;

    submesh.mIndexData.clear();
    submesh.mIndexData = {
      0, 1, 2,
      2, 3, 0
    };

    auto view = mSpace->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(meshName, submesh);

    mInstantiatedSprite = mRenderer->CreateModel(view, mesh);

    mInstantiatedSprite->mType = ShaderType::AlphaBlendShader;

    CreateTransform();
    mUBOModel.mDiffuseColor = mInstantiatedSprite->GetUBOModelData().mDiffuseColor;
    mInstantiatedSprite->UpdateUBOModel(mUBOModel);
    mInstantiatedSprite->UpdateUBOMaterial(&modelMaterial);
    mInstantiatedSprite->SetVisibility(mVisibility);
  }
}
