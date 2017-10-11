#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/Physics/CylinderCollider.hpp"
#include "YTE/Physics/SphereCollider.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.h"

#include <filesystem>

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / L"Models";

    std::vector<std::string> result;

    for (auto & p : filesystem::directory_iterator(finalPath))
    {
      std::string str = p.path().filename().generic_string();

      result.push_back(str);
    }

    return result;
  }

  YTEDefineType(Model)
  {
    YTERegisterType(Model);

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

    Model::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
    
    YTEBindProperty(&Model::GetMesh, &Model::SetMesh, "Mesh")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&Model::GetReload, &Model::SetReload, "Reload")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  Model::Model(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mConstructing(true)
    , mRenderer(nullptr)
    , mInstantiatedMesh(nullptr)
    , mUpdating(false)
  {
    auto engine = aSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();
    DeserializeByType<Model*>(aProperties, this, Model::GetStaticType());

    mConstructing = false;

    Create();
  }

  Model::~Model()
  {
    Destroy();
  }

  static bool FileCheck(const Path& aPath, const std::string& aDirectory, std::string &aFile)
  {
    if (0 == aFile.size())
    {
      return false;
    }

    std::experimental::filesystem::path pathName{ aPath.String() };
    pathName.append(aDirectory);
    pathName.append(aFile);
    return std::experimental::filesystem::exists(pathName);
  }
  
  void Model::Create()
  {
    auto meshName = RemoveExtension(mMesh);

    auto name = mOwner->GetName().c_str();

    if (false == FileCheck(Path::GetGamePath(), "Models", mMesh))
    {
      printf("Model (%s): Needs valid ModelName, provided: %s\n",
             name,
             mMesh.c_str());

      return;
    }

    auto window = mSpace->GetComponent<GraphicsView>()->GetWindow();

    mInstantiatedMesh = mRenderer->AddModel(window, mMesh);

    auto transform = mOwner->GetComponent<Transform>();

    if (mInstantiatedMesh && transform)
    {
      mInstantiatedMesh->mPosition = transform->GetTranslation();
      mInstantiatedMesh->mScale = transform->GetScale();
      mInstantiatedMesh->mRotation = transform->GetRotation();
    }

    SetUBO();
  }

  void Model::Destroy()
  {
    mInstantiatedMesh.reset();
  }

  void Model::Initialize()
  {
    mOwner->YTERegister(Events::PositionChanged, this, &Model::OnPositionChange);
    mOwner->YTERegister(Events::ScaleChanged, this, &Model::OnScaleChange);
    mOwner->YTERegister(Events::RotationChanged, this, &Model::OnRotationChange);

    auto transform = mOwner->GetComponent<Transform>();

    if (mInstantiatedMesh && transform)
    {
      mInstantiatedMesh->mPosition = transform->GetTranslation();
      mInstantiatedMesh->mScale = transform->GetScale();
      mInstantiatedMesh->mRotation = transform->GetRotation();
    }

    SetUBO();
  }

  void Model::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    SetUBO();
    mSpace->YTEDeregister(Events::FrameUpdate, this, &Model::Update);
    mUpdating = false;
  }

  void Model::OnPositionChange(const TransformChanged *aEvent)
  {
    if (mInstantiatedMesh)
    {
      mInstantiatedMesh->mPosition = aEvent->WorldPosition;
    }

    if (false == mUpdating)
    {
      mSpace->YTERegister(Events::FrameUpdate, this, &Model::Update);
      mUpdating = true;
    }
  }

  void Model::OnScaleChange(const TransformChanged *aEvent)
  {
    if (mInstantiatedMesh)
    {
      mInstantiatedMesh->mScale = aEvent->WorldScale;
    }

    if (false == mUpdating)
    {
      mSpace->YTERegister(Events::FrameUpdate, this, &Model::Update);
      mUpdating = true;
    }
  }

  void Model::OnRotationChange(const TransformChanged *aEvent)
  {
    if (mInstantiatedMesh)
    {
      mInstantiatedMesh->mRotation = aEvent->WorldRotation;
    }

    if (false == mUpdating)
    {
      mSpace->YTERegister(Events::FrameUpdate, this, &Model::Update);
      mUpdating = true;
    }
  }

  void Model::SetUBO()
  {
    if (mInstantiatedMesh)
    {
      mRenderer->UpdateModelTransformation(this);
    }
  }
}
