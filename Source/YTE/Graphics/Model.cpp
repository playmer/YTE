///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  YTEDefineEvent(ModelChanged);

  YTEDefineType(ModelChanged)
  {
    YTERegisterType(ModelChanged);
    YTEBindField(&ModelChanged::Object, "Object", PropertyBinding::Get);
  }


  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / L"Models";

    std::vector<std::string> result;

    result.push_back("None");
    result.push_back("cube.fbx");
    result.push_back("sphere.fbx");
    result.push_back("cylinder.fbx");
    result.push_back("plane.fbx");

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
    mOwner->YTERegister(Events::PositionChanged, this, &Model::TransformUpdate);
    mOwner->YTERegister(Events::RotationChanged, this, &Model::TransformUpdate);
    mOwner->YTERegister(Events::ScaleChanged, this, &Model::TransformUpdate);
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


  
  void Model::TransformUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    CreateTransform();

    if (mInstantiatedModel)
    {
      mInstantiatedModel->UpdateUBOModel(mUBOModel);
    }
  }



  void Model::SetMesh(std::string aName)
  {
    if (aName == mMeshName || aName == "None")
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

    ModelChanged modChange;
    modChange.Object = mOwner;
    mOwner->SendEvent(Events::ModelChanged, &modChange);
  }



  Mesh* Model::GetMesh()
  {
    if (mInstantiatedModel)
    {
      return mInstantiatedModel->GetMesh();
    }

    return nullptr;
  }



  void Model::Create()
  {
    std::string MeshName = RemoveExtension(mMeshName);
    std::string name = mOwner->GetName().c_str();

    bool success = FileCheck(Path::GetGamePath(), "Models", mMeshName);

    if (false == success)
    {
      success = FileCheck(Path::GetEnginePath(), "Models", mMeshName);
    }
    
    if (false == success)
    {
      printf("Model (%s): Model of name %s is not found.", name.c_str(), mMeshName.c_str());
      return;
    }

    CreateTransform();
    mInstantiatedModel = mRenderer->CreateModel(mWindow, mMeshName);
    if (mInstantiatedModel && mTransform)
    {
      mInstantiatedModel->UpdateUBOModel(mUBOModel);
    }
  }



  void Model::Destroy()
  {
    mInstantiatedModel.reset();
  }



  void Model::CreateTransform()
  {
    if (mTransform == nullptr)
    {
      return;
    }

    mUBOModel.mModelMatrix = glm::translate(glm::mat4(1.0f), mTransform->GetWorldTranslation());

    mUBOModel.mModelMatrix = mUBOModel.mModelMatrix * glm::toMat4(mTransform->GetWorldRotation());

    mUBOModel.mModelMatrix = glm::scale(mUBOModel.mModelMatrix, mTransform->GetWorldScale());
  }


  YTEDefineType(Animator::Animation)
  {
    YTERegisterType(Animator::Animation);

    YTEBindProperty(&Animator::Animation::GetSpeed, &Animator::Animation::SetSpeed, "Speed");
  }

  YTEDefineType(Animator)
  {
    YTERegisterType(Animator);

    Animator::GetStaticType()->AddAttribute<EditorHeaderList>(&Animator::Lister, "Animations");
  }

  Animator::Animation::Animation(std::string aName) : mName(aName), mSpeed(0.0f)
  {
  }

  float Animator::Animation::GetSpeed()
  {
    return mSpeed;
  }


  void Animator::Animation::SetSpeed(float aSpeed)
  {
    mSpeed = aSpeed;
  }


  Animator::Animator(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<Animator*>(aProperties, this, Animator::GetStaticType());
  }

  std::vector<std::pair<YTE::Object*, std::string>> Animator::Lister(YTE::Object *aSelf)
  {
    auto self = static_cast<Animator*>(aSelf);

    std::vector<std::pair<YTE::Object*, std::string>> animations;

    for (auto &animation : self->mAnimations)
    {
      animations.emplace_back(std::make_pair(animation.first, animation.second));
    }

    return animations;
  }

  Animator::Animation* Animator::AddAnimation(std::string aName)
  {
    Animation *anim = new Animation(aName);

    mAnimations.emplace_back(std::make_pair(anim, aName));

    return anim;
  }

}

