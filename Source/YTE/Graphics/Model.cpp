///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Animation.hpp"

#include "YTE/Utilities/Utilities.hpp"


namespace YTE
{
  YTEDefineEvent(ModelChanged);

  YTEDefineType(ModelChanged)
  {
    RegisterType<ModelChanged>();
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

  static std::vector<std::string> PopulateShadingDropDown(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);
    return { "Standard", "Additive Blending", "ShaderNoCull", "Alpha Blend" };
  }



  YTEDefineType(Model)
  {
    RegisterType<Model>();

    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&Model::GetMeshName, &Model::SetMeshName, "Mesh")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&Model::GetShading, &Model::SetShading, "Shading")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateShadingDropDown);

    YTEBindProperty(&Model::GetReload, &Model::SetReload, "Reload")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    //YTEBindProperty(&Model::GetInstanced, &Model::SetInstanced, "Instanced")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>()
    //  .SetDocumentation("Will use/not use instancing for this mesh. (Will apply to all Models using this mesh.)");
  }



  /////////////////////////////////
  // Model Class Functions
  /////////////////////////////////
  Model::Model(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : BaseModel{ aOwner, aSpace, aProperties }
    , mRenderer(nullptr)
    , mWindow(nullptr)
    , mMeshName("")
    , mTransform(nullptr)
    , mInstantiatedModel(nullptr)
    , mConstructing(true)
    , mBackfaceCulling(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }



  Model::~Model()
  {
    Destroy();
  }



  void Model::NativeInitialize()
  {
    mEngine = mSpace->GetEngine();
    mRenderer = mSpace->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();

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



  bool Model::CanAnimate()
  {
    if (mInstantiatedModel)
    {
      return mInstantiatedModel->GetMesh()->CanAnimate();
    }
    else
    {
      return false;
    }
  }



  void Model::TransformUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    CreateTransform();

    if (mInstantiatedModel)
    {
      mUBOModel.mDiffuseColor = mInstantiatedModel->GetUBOModelData().mDiffuseColor;
      mInstantiatedModel->UpdateUBOModel(mUBOModel);
    }
  }



  void Model::SetMesh(std::string &aName)
  {
    if (aName.empty() || aName == mMeshName)
    {
      return;
    }

    std::experimental::filesystem::path path(aName);
    std::string exten = path.extension().generic_string();

    if (aName == "None")
    {
      mMeshName = aName;
      Destroy();
    }
    else if (exten != ".fbx")
    {
      return;
    }
    else
    {
      mMeshName = aName;

      if (mConstructing)
      {
        return;
      }
      Destroy();
      Create();
    }
  }



  void Model::SetShading(std::string const &aName)
  {
    if (aName.empty())
    {
      return;
    }

    if (aName == "Standard")
    {
      mShadingName = "Standard";
    }
    else if (aName == "Additive Blending")
    {
      mShadingName = "Additive Blending";
    }
    else if (aName == "ShaderNoCull")
    {
      mShadingName = "ShaderNoCull";
    }
    else if (aName == "Alpha Blend")
    {
      mShadingName = "Alpha Blend";
    }

    if (!mInstantiatedModel)
    {
      return;
    }

    if (aName == "Standard")
    {
      mInstantiatedModel->mType = ShaderType::Triangles;
    }
    else if (aName == "Additive Blending")
    {
      mInstantiatedModel->mType = ShaderType::AdditiveBlendShader;
    }
    else if (aName == "ShaderNoCull")
    {
      mInstantiatedModel->mType = ShaderType::ShaderNoCull;
    }
    else if (aName == "Alpha Blend")
    {
      mInstantiatedModel->mType = ShaderType::AlphaBlendShader;
    }
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
    YTEProfileFunction();
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
    auto view = mSpace->GetComponent<GraphicsView>();

    mInstantiatedModel = mRenderer->CreateModel(view, mMeshName);

    if (mInstantiatedModel)
    {
      SetShading(mShadingName);
    }

    if (mInstantiatedModel && mTransform)
    {
      mUBOModel.mDiffuseColor = mInstantiatedModel->GetUBOModelData().mDiffuseColor;
      mInstantiatedModel->UpdateUBOModel(mUBOModel);
      mInstantiatedModel->SetVisibility(mVisibility);
    }

    if (mInstantiatedModel->GetMesh()->CanAnimate())
    {
      mInstantiatedModel->SetDefaultAnimationOffset();
    }

    ModelChanged modChange;
    modChange.Object = mOwner;
    mOwner->SendEvent(Events::ModelChanged, &modChange);
  }


  void Model::Destroy()
  {
    mInstantiatedModel.reset();
  }

  void Model::SetInstanced(bool aInstanced)
  {
    if (mInstantiatedModel)
    {
      mInstantiatedModel->SetInstanced(aInstanced);
    }
  }

  bool Model::GetInstanced()
  {
    if (mInstantiatedModel)
    {
      return mInstantiatedModel->GetInstanced();
    }
    else
    {
      return false;
    }
  }

  void Model::CreateTransform()
  {
    if (mTransform == nullptr)
    {
      return;
    }

    mUBOModel.mModelMatrix = mTransform->GetTransformMatrix();
  }

}
