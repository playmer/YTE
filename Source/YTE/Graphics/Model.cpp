///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Utilities/Utilities.h"

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

    YTEBindProperty(&Model::GetMeshName, &Model::SetMeshName, "Mesh")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&Model::GetReload, &Model::SetReload, "Reload")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }



  /////////////////////////////////
  // Model Class Functions
  /////////////////////////////////
  Model::Model(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mRenderer(nullptr)
    , mWindow(nullptr)
    , mMeshName("")
    , mTransform(nullptr)
    , mInstantiatedModel(nullptr)
    , mConstructing(true)
  {
    mRenderer = aSpace->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mWindow = aSpace->GetComponent<GraphicsView>()->GetWindow();

    DeserializeByType<Model*>(aProperties, this, Model::GetStaticType());
  }



  Model::~Model()
  {
    Destroy();
  }



  void Model::Initialize()
  {
    mOwner->YTERegister(Events::PositionChanged, this, &Model::PositionUpdate);
    mOwner->YTERegister(Events::RotationChanged, this, &Model::RotationUpdate);
    mOwner->YTERegister(Events::ScaleChanged, this, &Model::ScaleUpdate);
    mTransform = mOwner->GetComponent<Transform>();
    mConstructing = false;
    Create();
  }



  void Model::Reload()
  {
    if (mConstructing)
    {
      return;
    }
    Destroy();
    Create();
  }


  
  void Model::PositionUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    CreateTransform();

    if (mInstantiatedModel)
    {
      mInstantiatedModel->UpdateUBOPerModel(mUBOPerModel);
    }
  }



  void Model::RotationUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    CreateTransform();

    if (mInstantiatedModel)
    {
      mInstantiatedModel->UpdateUBOPerModel(mUBOPerModel);
    }
  }



  void Model::ScaleUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    CreateTransform();

    if (mInstantiatedModel)
    {
      mInstantiatedModel->UpdateUBOPerModel(mUBOPerModel);
    }
  }



  void Model::SetMesh(std::string aName)
  {
    if (aName == mMeshName)
    {
      return;
    }

    mMeshName = aName;

    if (mConstructing)
    {
      return;
    }
    Destroy();
    Create();
  }


  // file scoped function to check for file path correctness
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



  std::shared_ptr<Mesh> Model::GetMesh()
  {
    return mInstantiatedModel->GetMesh();
  }



  void Model::Create()
  {
    std::string MeshName = RemoveExtension(mMeshName);
    std::string name = mOwner->GetName().c_str();

    if (false == FileCheck(Path::GetGamePath(), "Models", mMeshName))
    {
      printf("Model (%s): Model of name %s is not found.", name.c_str(), mMeshName.c_str());
      return;
    }

    CreateTransform();
    mInstantiatedModel = mRenderer->CreateModel(mWindow, mMeshName);
    if (mInstantiatedModel && mTransform)
    {
      mInstantiatedModel->UpdateUBOPerModel(mUBOPerModel);
    }
  }



  void Model::Destroy()
  {
    mRenderer->DestroyModel(mWindow, mInstantiatedModel);
    mInstantiatedModel.reset();
  }



  void Model::CreateTransform()
  {
    if (mTransform == nullptr)
    {
      return;
    }

    mUBOPerModel.mModelMatrix = glm::translate(glm::mat4(1.0f), mTransform->GetTranslation());

    mUBOPerModel.mModelMatrix = mUBOPerModel.mModelMatrix * glm::toMat4(mTransform->GetRotation());

    mUBOPerModel.mModelMatrix = glm::scale(mUBOPerModel.mModelMatrix, mTransform->GetScale());
  }
}
