/******************************************************************************/
/*!
\author Joshua T. Fisher
\par    email: j.fisher\@digipen.edu
\date   2015-10-26
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include <memory>
#include <iostream>
#include <fstream>

#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.hpp"

#include "YTE/WWise/WWiseView.hpp"

namespace YTE
{




  //template <typename Return, typename Enable = void>
  //struct FunctionBinding {};
  //
  //
  /////////////////////////////////////////////////////////////////////////////////
  //// Free Functions
  /////////////////////////////////////////////////////////////////////////////////
  ////Returns Something
  //template <typename Return, typename... Arguments>
  //struct FunctionBinding<Return(*)(Arguments...), typename std::enable_if<std::is_void<Return>::value == false>::type >
  //{
  //  using FunctionSignature = Return(*)(Arguments...);
  //  using CallingType = Any(*)(std::vector<Any>&);
  //
  //  template <FunctionSignature BoundFunc>
  //  static Any Caller(std::vector<Any>& aArguments)
  //  {
  //    size_t i = 0;
  //
  //    // We get a warning for functions that don't have arguments and thus don't use these.
  //    YTEUnusedArgument(aArguments);
  //    YTEUnusedArgument(i);
  //
  //    Return capture = BoundFunc(aArguments.at(i++).As<Arguments>()...);
  //    Any toReturn{ capture, TypeId<Return>(), false == std::is_reference<Return>::value };
  //    return toReturn;
  //  }
  //
  //  template <FunctionSignature BoundFunc>
  //  static std::unique_ptr<Function> BindFunction(const char *name)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), nullptr, true);
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(Caller<BoundFunc>);
  //
  //    return std::move(function);
  //  }
  //};
  //
  ////Returns void
  //template <typename Return, typename... Arguments>
  //struct FunctionBinding<Return(*)(Arguments...), typename std::enable_if<std::is_void<Return>::value>::type >
  //{
  //  using FunctionSignature = Return(*)(Arguments...);
  //  using CallingType = Any(*)(std::vector<Any>&);
  //
  //  template <FunctionSignature BoundFunc>
  //  static Any Caller(std::vector<Any>& aArguments)
  //  {
  //    size_t i = 0;
  //
  //    // We get a warning for functions that don't have arguments and thus don't use these.
  //    YTEUnusedArgument(aArguments);
  //    YTEUnusedArgument(i);
  //
  //    BoundFunc(aArguments.at(i++).As<Arguments>()...);
  //    return Any();
  //  }
  //
  //  static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), nullptr, true);
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(aCaller);
  //
  //    return std::move(function);
  //  }
  //
  //  template <FunctionSignature BoundFunc>
  //  static std::unique_ptr<Function> BindFunction(const char *name)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), nullptr, true);
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(Caller<BoundFunc>);
  //
  //    return std::move(function);
  //  }
  //};
  //
  /////////////////////////////////////////////////////////////////////////////////
  //// Member Functions
  /////////////////////////////////////////////////////////////////////////////////
  ////Returns Something
  //template <typename Return, typename ObjectType, typename... Arguments>
  //struct FunctionBinding<Return(ObjectType::*)(Arguments...), typename std::enable_if<std::is_void<Return>::value == false>::type>
  //{
  //  using FunctionSignature = Return(ObjectType::*)(Arguments...);
  //  using CallingType = Any(*)(std::vector<Any>&);
  //
  //  template <FunctionSignature BoundFunc>
  //  static Any Caller(std::vector<Any>& aArguments)
  //  {
  //    auto self = aArguments.at(0).As<ObjectType*>();
  //
  //    size_t i = 1;
  //
  //    // We get a warning for functions that don't have arguments and thus don't use these.
  //    YTEUnusedArgument(aArguments);
  //    YTEUnusedArgument(i);
  //
  //    Return capture = (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
  //    Any toReturn{ capture, TypeId<Return>(), false == std::is_reference<Return>::value };
  //    return toReturn;
  //  }
  //
  //
  //  template <FunctionSignature BoundFunc>
  //  static std::unique_ptr<Function> BindFunction(const char *name)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(Caller<BoundFunc>);
  //
  //    return std::move(function);
  //  }
  //
  //  static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(aCaller);
  //
  //    return std::move(function);
  //  }
  //};
  //
  //// Returns Void
  //template <typename Return, typename ObjectType, typename... Arguments>
  //struct FunctionBinding<Return(ObjectType::*)(Arguments...), typename std::enable_if<std::is_void<Return>::value>::type>
  //{
  //  using FunctionSignature = Return(ObjectType::*)(Arguments...);
  //  using CallingType = Any(*)(std::vector<Any>&);
  //
  //  template <FunctionSignature BoundFunc>
  //  static Any Caller(std::vector<Any>& aArguments)
  //  {
  //    auto self = aArguments.at(0).As<ObjectType*>();
  //
  //    size_t i = 1;
  //
  //    // We get a warning for functions that don't have arguments and thus don't use these.
  //    YTEUnusedArgument(aArguments);
  //    YTEUnusedArgument(i);
  //
  //    (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
  //
  //    return Any();
  //  }
  //
  //  static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(aCaller);
  //
  //    return std::move(function);
  //  }
  //
  //  template <FunctionSignature BoundFunc>
  //  static std::unique_ptr<Function> BindFunction(const char *name)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(Caller<BoundFunc>);
  //
  //    return std::move(function);
  //  }
  //};
  //
  //
  /////////////////////////////////////////////////////////////////////////////////
  //// Const Member Functions
  /////////////////////////////////////////////////////////////////////////////////
  ////Returns Something
  //template <typename Return, typename ObjectType, typename... Arguments>
  //struct FunctionBinding<Return(ObjectType::*)(Arguments...) const, typename std::enable_if<std::is_void<Return>::value == false>::type>
  //{
  //  using FunctionSignature = Return(ObjectType::*)(Arguments...) const;
  //  using CallingType = Any(*)(std::vector<Any>&);
  //
  //  template <FunctionSignature BoundFunc>
  //  static Any Caller(std::vector<Any>& aArguments)
  //  {
  //    auto self = aArguments.at(0).As<ObjectType*>();
  //
  //    size_t i = 1;
  //
  //    // We get a warning for functions that don't have arguments and thus don't use these.
  //    YTEUnusedArgument(aArguments);
  //    YTEUnusedArgument(i);
  //
  //    Return capture = (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
  //    Any toReturn{ capture, TypeId<Return>(), false == std::is_reference<Return>::value };
  //    return toReturn;
  //  }
  //
  //
  //  template <FunctionSignature BoundFunc>
  //  static std::unique_ptr<Function> BindFunction(const char *name)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(Caller<BoundFunc>);
  //
  //    return std::move(function);
  //  }
  //
  //  static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(aCaller);
  //
  //    return std::move(function);
  //  }
  //};
  //
  ////Returns void
  //template <typename Return, typename ObjectType, typename... Arguments>
  //struct FunctionBinding<Return(ObjectType::*)(Arguments...) const, typename std::enable_if<std::is_void<Return>::value>::type>
  //{
  //  using FunctionSignature = Return(ObjectType::*)(Arguments...) const;
  //  using CallingType = Any(*)(std::vector<Any>&);
  //
  //  template <FunctionSignature BoundFunc>
  //  static Any Caller(std::vector<Any>& aArguments)
  //  {
  //    auto self = aArguments.at(0).As<ObjectType*>();
  //
  //    size_t i = 1;
  //
  //    // We get a warning for functions that don't have arguments and thus don't use these.
  //    YTEUnusedArgument(aArguments);
  //    YTEUnusedArgument(i);
  //
  //    (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
  //
  //    return Any();
  //  }
  //
  //  static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(aCaller);
  //
  //    return std::move(function);
  //  }
  //
  //  template <FunctionSignature BoundFunc>
  //  static std::unique_ptr<Function> BindFunction(const char *name)
  //  {
  //    auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
  //    function->AddParameter(TypeId<ObjectType*>());
  //    ParseArguments<Arguments...>::Parse(function.get());
  //
  //    function->SetCaller(Caller<BoundFunc>);
  //
  //    return std::move(function);
  //  }
  //};
  //
  //template <>
  //struct FunctionBinding<nullptr_t>
  //{
  //  template <nullptr_t BoundFunc>
  //  static std::unique_ptr<Function> BindFunction(const char *name)
  //  {
  //    YTEUnusedArgument(name);
  //
  //    return std::unique_ptr<Function>();
  //  }
  //};



































































  template <typename tType>
  class TypeBuilder
  {
  public:
    TypeBuilder()
      : mType{ TypeId<tType>() }
    {

    }

    Type* GetType()
    {
      return mType;
    }

    template <typename FunctionSignature>
    class FunctionBuilder
    {
    public:
      FunctionBuilder(Function *aFunction)
        : mFunction{ aFunction }
      {

      }

      Function* GetFunction()
      {
        return mFunction;
      }

      template <typename... tArguments>
      FunctionBuilder& SetParameterNames(tArguments &&...aNames)
      {
        constexpr size_t passedNamesSize = sizeof...(aNames);
        constexpr size_t funcNamesSize = CountFunctionArguments<FunctionSignature>::template Size();
        static_assert(0 != funcNamesSize, "You needn't set the parameter names for this function, as there are no parameters.");

        static_assert(passedNamesSize == funcNamesSize,
                      "If passing names of function parameters you must pass either exactly as many names as there are arguments, or 0.");

        mFunction->SetParameterNames({ std::forward<tArguments>(aNames)... });

        return *this;
      }

      FunctionBuilder& SetDocumentation(const char *aString)
      {
        mFunction->SetDocumentation(aString);

        return *this;
      }

      template <typename tType, typename... tArguments>
      FunctionBuilder& AddAttribute(tArguments &&...aArguments)
      {
        mFunction->AddAttribute<tType>(std::forward<tArguments>(aArguments)...);

        return *this;
      }

    private:
      Function* mFunction;
    };

    template <typename FunctionSignature, FunctionSignature aBoundFunction>
    FunctionBuilder<FunctionSignature> BindFunction(const char *name)
    {
      auto function = Binding<FunctionSignature>:: template BindFunction<aBoundFunction>(name);
      function->SetOwningType(TypeId<tType>());

      auto ptr = TypeId<tType>()->AddFunction(std::move(function));

      return FunctionBuilder<FunctionSignature>{ ptr };
    }

    //template <typename FunctionSignature, FunctionSignature aBoundFunction, size_t aSize>
    //Function& BindFunction(const char *name, std::array<const char *, aSize> aParameterNames)
    //{
    //  static_assert(aSize == CountFunctionArguments<FunctionSignature>::template Size() || aSize == 0,
    //                "If passing names of function parameters you must pass either exactly as many names as there are arguments, or 0.");
    //
    //  auto function = Binding<FunctionSignature>:: template BindFunction<aBoundFunction>(name);
    //  //function->SetParameterNames(aParameterNames);
    //  function->SetOwningType(TypeId<tType>());
    //
    //  auto ptr = TypeId<tType>()->AddFunction(std::move(function));
    //
    //  return *ptr;
    //}
    
    template <typename GetterFunctionSignature, GetterFunctionSignature GetterFunction,
              typename SetterFunctionSignature, SetterFunctionSignature SetterFunction>
    Property& BindProperty(const char *aName)
    {
      std::unique_ptr<Function> getter;
      std::unique_ptr<Function> setter;
    
      //if constexpr (false == std::is_same_v<GetterFunctionSignature, nullptr_t>)
      //{
      //  getter = Binding<GetterFunctionSignature>:: template BindFunction<GetterFunction>("Getter");
      //}
      //
      //if constexpr (false == std::is_same_v<SetterFunctionSignature, nullptr_t>)
      //{
      //  setter = Binding<SetterFunctionSignature>:: template BindFunction<SetterFunction>("Setter");
      //}

      getter = Binding<GetterFunctionSignature>::BindFunction<GetterFunction>("Getter");
      setter = Binding<SetterFunctionSignature>::BindFunction<SetterFunction>("Setter");

      auto property = std::make_unique<Property>(aName, std::move(getter), std::move(setter));
    
      auto ptr = TypeId<tType>()->AddProperty(std::move(property));
    
      return *ptr;
    }

    template <typename FieldPointerType, FieldPointerType aFieldPointer>
    Field& BindField(char const *aName, PropertyBinding aBinding)
    {
      using FieldType = typename DecomposeFieldPointer<FieldPointerType>::FieldType;

      std::unique_ptr<Function> getter;
      std::unique_ptr<Function> setter;

      if (PropertyBinding::Get == aBinding || PropertyBinding::GetSet == aBinding)
      {
        getter = Binding<FieldType(tType::*)()>::BindPassedFunction("Getter", Field::Getter<FieldPointerType, aFieldPointer>);
      }

      if (PropertyBinding::Set == aBinding || PropertyBinding::GetSet == aBinding)
      {
        setter = Binding<void(tType::*)(FieldType)>::BindPassedFunction("Setter", Field::Setter<FieldPointerType, aFieldPointer>);
      }

      auto field = std::make_unique<Field>(aName, std::move(getter), std::move(setter));

      auto type = TypeId<typename DecomposeFieldPointer<FieldPointerType>::FieldType>();

      field->SetPropertyType(type);
      field->SetOffset(Field::GetOffset<FieldPointerType, aFieldPointer>());

      auto ptr = TypeId<tType>()->AddField(std::move(field));
      return *ptr;
    }

  private:
    Type * mType;
  };

  constexpr auto NoSetterFunc()
  {
    return nullptr;
  }

  inline constexpr nullptr_t NoSetter = nullptr;

  YTEDefineType(Space)
  {
    YTERegisterType(Space);

    TypeBuilder<Space> builder;

    builder.BindFunction<decltype(&Space::LoadLevel), &Space::LoadLevel>("x")
      .SetParameterNames("aLevel", "CheckRunInEditor")
      .SetDocumentation("Loads a level within the current space on the next frame. Current level will be torn down.");

    builder.BindFunction<decltype(&Space::SaveLevel), &Space::SaveLevel>("x")
      .SetParameterNames("aLevelName")
      .SetDocumentation("Saves a level to the given file");


    builder.BindFunction<decltype(&Space::Remove), &Space::Remove>("x")
      .SetDocumentation("Saves a level to the given file");
      
    YTEBindProperty(&Space::IsPaused, &Space::SetPaused, "Paused").Description() = "Sets if the space is paused or not.";

    YTEBindProperty(&Space::GetEngine, YTENoSetter, "Engine");
    //builder.BindProperty<decltype(&Space::GetEngine), &Space::GetEngine, decltype(NoSetter), NoSetter>("x");

    builder.BindField<decltype(&Space::mStartingLevel), &Space::mStartingLevel>("x", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }


  Space::Space(Engine *aEngine, RSValue *aProperties)
    : Composition(aEngine, this, aEngine)
    , mLevelToLoad(nullptr)
  {
    if (false == mEngine->IsEditor())
    {
      mEngine->GetWindow()->YTERegister(Events::WindowFocusLostOrGained, 
                                        this, 
                                        &Space::WindowLostOrGainedFocusHandler);
      mEngine->GetWindow()->YTERegister(Events::WindowMinimizedOrRestored, 
                                        this, 
                                        &Space::WindowMinimizedOrRestoredHandler);
    }


    mEngine->YTERegister(Events::LogicUpdate, this, &Space::Update);

    if (nullptr != aProperties)
    {
      DeserializeByType(aProperties, this, TypeId<Space>());
    }
  }

  void Space::Initialize()
  {
    YTEProfileFunction();
    InitializeEvent event;
    event.CheckRunInEditor = mIsEditorSpace;
    Initialize(&event);
  }

  // Loads a level into the current Space. If already loaded, destroys 
  // the current Space and loads level in place.
  void Space::Load()
  {
    YTEProfileFunction();
    if (mStartingLevel.Empty())
    {
      CreateBlankLevel("NewLevel");
    }
    else
    {
      Load(mEngine->GetLevel(mStartingLevel));
      mLevelName = mStartingLevel;
    }
  }

  // Loads a level into the current Space. If already loaded, destroys 
  // the current Space and loads level in place.
  void Space::Load(RSValue *aLevel, bool aInitialize)
  {
    YTEProfileFunction();
    mCompositions.Clear();
    ComponentClear();
      
    if (nullptr != aLevel)
    {
      Deserialize(aLevel);
    }
    else
    {
      printf("We could not deserialize the level provided.\n");
    }
      
    if (aInitialize)
    {
      Initialize();
    }
      
    mLoading = false;
  }


  void Space::Initialize(InitializeEvent *aEvent)
  {
    YTEProfileFunction();
    Composition::NativeInitialize(aEvent);
    Composition::PhysicsInitialize(aEvent);
    Composition::Initialize(aEvent);
    Composition::Start(aEvent);

    mShouldIntialize = true;
  }

  // Updates the Space to the current frame.
  void Space::Update(LogicUpdate *aEvent)
  {
    YTEProfileFunction();
    if (mLoading)
    {
      mLevelName = mLoadingName;
      SetName(mLoadingName);
      Load(mLevelToLoad);
    }

    SendEvent(Events::DeletionUpdate, aEvent);
      
    if (mPaused == false)
    {
      SendEvent(Events::PhysicsUpdate, aEvent);
    }

    // TODO: Move the frame update calls to the graphics system
    SendEvent(Events::FrameUpdate, aEvent);

    // Don't send the LogicUpdate Event if the space is paused.
    if (mPaused == false)
    {
      SendEvent(Events::LogicUpdate, aEvent);
    }
  }
    
  // Cleans up anything in the Space.
  Space::~Space() 
  {
  }

  void Space::CreateBlankLevel(const String& aLevelName)
{
    mCompositions.Clear();
    ComponentClear();

    mLevelName = aLevelName;

    AddComponent<WWiseView>();
    auto graphicsView = AddComponent<GraphicsView>();
    graphicsView->ChangeWindow("Yours Truly Engine");
    graphicsView->NativeInitialize();
    
    AddComponent<PhysicsSystem>();
    AddComponent<ActionManager>();

    auto camera = AddComposition<Composition>("Camera", mEngine, this);
    camera->SetOwner(this);
    camera->AddComponent<Camera>();
    camera->AddComponent<Orientation>();
    camera->AddComponent<Transform>();

    Initialize();

    mLoading = false;
  }

  void Space::LoadLevel(String &level, bool aCheckRunInEditor)
  {
    mCheckRunInEditor = aCheckRunInEditor;
    mLoading = true;
    mLevelToLoad = mEngine->GetLevel(level);
    mLoadingName = level;
  }

  void Space::SaveLevel(String &aLevelName)
  {
    // TODO (Josh): Fix
    RSAllocator allocator;
    auto value = Serialize(allocator);

    std::string levelNameTemp(aLevelName.c_str());
    std::wstring level{levelNameTemp.begin(), levelNameTemp.end()};

    std::string path = YTE::Path::GetGamePath().String();
    std::wstring pathWStr{ path.begin(), path.end() };
    
    level = pathWStr + L"Levels/" + level + L".json";
    
    level = std::experimental::filesystem::canonical(level, cWorkingDirectory);
    
    RSStringBuffer sb;
    RSPrettyWriter writer(sb);
    value.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    std::string levelInJson = sb.GetString();
    
    std::ofstream levelToSave;
    levelToSave.open(level);
    levelToSave << levelInJson;
    levelToSave.close();
  }


  Space* Space::AddChildSpace(String aLevelName)
  {
    auto newSpace = AddComposition<Space>(aLevelName, mEngine, nullptr);
    newSpace->mOwner = this;
    newSpace->Load(mEngine->GetLevel(aLevelName));
    auto ourView = GetComponent<GraphicsView>();
    auto newView = newSpace->GetComponent<GraphicsView>();

    if (ourView && newView)
    {
      newView->ChangeWindow(ourView->GetWindow());
    }

    return newSpace;
  }


  // TODO (Josh): Abstract or move to another handler.
  void Space::WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent)
  {
    YTEUnusedArgument(aEvent);

    //if ((mFocusHandled == false) && (aEvent->Focused == false) && !mEngine->GetWindow()->IsMinimized())
    //{
    //  //std::cout << "Set mPriorToMinimize" << std::endl;
    //  mPriorToMinimize = mPaused;
    //  mPaused = true;
    //  mFocusHandled = true;
    //}
    //else if ((aEvent->Focused == true) && !mEngine->GetWindow()->IsMinimized())
    //{
    //  mPaused = mPriorToMinimize;
    //  mFocusHandled = false;
    //}
  }

  void Space::WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent)
  {
    YTEUnusedArgument(aEvent);

    //if (aEvent->Minimized && !mEngine->GetWindow()->IsNotFocused())
    //{
    //  //std::cout << "Set mPriorToMinimize" << std::endl;
    //  mPriorToMinimize = mPaused;
    //  mPaused = true;
    //}
    //else if (!aEvent->Minimized && !mEngine->GetWindow()->IsNotFocused())
    //{
    //  mPaused = mPriorToMinimize;
    //  mFocusHandled = false;
    //}
  }
} // End yte namespace
