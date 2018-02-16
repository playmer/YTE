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


  YTEDefineType(Sprite)
  {
    YTERegisterType(Sprite);

    YTEBindProperty(&Sprite::GetTexture, &Sprite::SetTexture, "Texture")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&Sprite::GetColumns, &Sprite::SetColumns, "Columns")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&Sprite::GetRows, &Sprite::SetRows, "Rows")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&Sprite::GetFrames, &Sprite::SetFrames, "Frames")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&Sprite::GetSpeed, &Sprite::SetSpeed, "Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("How many seconds it will take for the full animation to run.");

    YTEBindProperty(&Sprite::GetAnimating, &Sprite::SetAnimating, "Animating")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  Sprite::Sprite(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component{ aOwner, aSpace }
    , mConstructing{ true }
    , mAnimating{ false }
    , mColumns{ 1 }
    , mRows{ 1 }
    , mFrames{ 1 }
    , mSpeed{ 1.0f }
    , mTimeAccumulated{ 0.0 }
    , mCurrentIndex{ 0 }
  {
    DeserializeByType(aProperties, this, GetStaticType());

    mConstructing = false;
  }

  Sprite::~Sprite()
  {

  }

  void Sprite::Update(LogicUpdate *aUpdate)
  {
    auto delta = mSpeed / mFrames;

    if (mInstantiatedSprite)
    {
      mTimeAccumulated += aUpdate->Dt;

      auto xWidth = 1.0f / mColumns;
      auto yWidth = 1.0f / mRows;

      glm::vec3 uv0 = { 0.0f,   0.0f, 0.0f };
      glm::vec3 uv1 = { xWidth, 0.0f, 0.0f };
      glm::vec3 uv2 = { xWidth, yWidth , 0.0f };
      glm::vec3 uv3 = { 0.0f,   yWidth , 0.0f };

      if (mTimeAccumulated > delta)
      {
        mTimeAccumulated = 0.0;
        auto column = mCurrentIndex % mColumns;
        auto row = mCurrentIndex / mColumns;

        

        printf("Row: %d, Column: %d\n", row, column);

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
      mSpace->YTERegister(Events::LogicUpdate, this, &Sprite::Update);
    }
    else
    {
      mSpace->YTEDeregister(Events::LogicUpdate, this, &Sprite::Update);
    }
  }

  void Sprite::Initialize()
  {
    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    mTransform = mOwner->GetComponent<Transform>();

    mOwner->YTERegister(Events::PositionChanged, this, &Sprite::TransformUpdate);
    mOwner->YTERegister(Events::RotationChanged, this, &Sprite::TransformUpdate);
    mOwner->YTERegister(Events::ScaleChanged, this, &Sprite::TransformUpdate);

    CreateSprite();
  }


  void Sprite::TransformUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

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

    Submesh submesh;

    Vertex vert0;
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;

    vert0.mPosition = { -0.5, -0.5, 0.0 };
    vert0.mTextureCoordinates = { 0.0f, 0.0f, 0.0f };
    vert1.mPosition = { 0.5, -0.5, 0.0 };
    vert1.mTextureCoordinates = { 1.0f, 0.0f, 0.0f };
    vert2.mPosition = { 0.5, 0.5, 0.0 };
    vert2.mTextureCoordinates = { 1.0f, 1.0f, 0.0f };
    vert3.mPosition = { -0.5, 0.5, 0.0 };
    vert3.mTextureCoordinates = { 0.0f, 1.0f, 0.0f };

    std::vector<u32> mIndices{
      0, 1, 2,
      2, 3, 0
    };

    submesh.mDiffuseMap = mTextureName;
    submesh.mDiffuseType = TextureViewType::e2D;
    submesh.mShaderSetName = "Sprite";

    submesh.mCullBackFaces = false;

    submesh.mVertexBuffer.emplace_back(vert0);
    submesh.mVertexBuffer.emplace_back(vert1);
    submesh.mVertexBuffer.emplace_back(vert2);
    submesh.mVertexBuffer.emplace_back(vert3);

    submesh.mIndexBuffer = std::move(mIndices);

    submesh.mVertexBufferSize = submesh.mVertexBuffer.size() * sizeof(Vertex);
    submesh.mIndexBufferSize = submesh.mIndexBuffer.size() * sizeof(u32);

    std::vector<Submesh> submeshes{ submesh };

    auto view = mSpace->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(view, meshName, submeshes);

    mInstantiatedSprite = mRenderer->CreateModel(view, mesh);

    mInstantiatedSprite->mUseAlphaBlending = true;
    mInstantiatedSprite->mUseAdditiveBlending = true;

    CreateTransform();
    mUBOModel.mDiffuseColor = mInstantiatedSprite->GetUBOModelData().mDiffuseColor;
    mInstantiatedSprite->UpdateUBOModel(mUBOModel);
  }
}
