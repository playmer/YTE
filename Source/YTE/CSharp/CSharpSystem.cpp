#include <filesystem>
#include <iostream>
#include <fstream>
#include <set>

#include <mono/jit/jit.h> 
#include <mono/metadata/appdomain.h> 
#include <mono/metadata/assembly.h> 
#include <mono/metadata/mono-config.h> 
#include <mono/metadata/mono-debug.h> 
#include <mono/metadata/debug-helpers.h> 
#include <mono/utils/mono-logger.h> 

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
 
#include "YTE/CSharp/CSharpSystem.hpp"
#include "YTE/CSharp/CSharp_CodeGenerator.hpp"

#include "YTE/WWise/WWiseSystem.hpp"


namespace YTE 
{ 
  namespace fs = std::experimental::filesystem;

  YTEDefineType(CSharpRedirect)
  {
    YTERegisterType(CSharpRedirect);
  }

  YTEDefineType(CSharpClass)
  {
    YTERegisterType(CSharpClass);
  }

  YTEDefineType(CSharpFunction)
  {
    YTERegisterType(CSharpFunction);
  }

  // Just doing what mono_object_unbox does, but such that it can be
  // inlined and returns the type we want.
  template <typename tType>
  tType* MonoUnbox(MonoObject *aObject)
  {
    return (tType*)((byte*)aObject) + sizeof(MonoObject);
  }

  template <typename tType>
  Any MonoUnboxRedirect(MonoObject *aObject)
  {
    auto value = *MonoUnbox<tType>(aObject);
    return Any{ value };
  }

  MonoObject* MonoBoxRedirect(Any aObject, CSharpRedirect *aClass)
  {
    u64 ptr = 0;

    if (nullptr != aObject.mType->GetReferenceTo() || 
        nullptr != aObject.mType->GetPointerTo())
    {
      ptr = reinterpret_cast<u64>(aObject.GetData());
    }
    else
    {

    }

    MonoObject *obj = mono_object_new(mono_domain_get(), aClass->GetMonoClass());
    MonoException *except;

    aClass->GetConstructor()(obj, ptr, true, &except);

    return obj;
  }

  MonoObject* MonoBoxRedirectStdString(Any aObject, CSharpRedirect *aClass)
  {
    auto str = aObject.As<std::string>();
    return reinterpret_cast<MonoObject*>(mono_string_new(mono_domain_get(), str.c_str()));
  }

  MonoObject* MonoBoxRedirectYTEString(Any aObject, CSharpRedirect *aClass)
  {
    auto str = aObject.As<String>();
    return reinterpret_cast<MonoObject*>(mono_string_new(mono_domain_get(), str.c_str()));
  }

  template <>
  Any MonoUnboxRedirect<std::string>(MonoObject *aObject)
  {
    auto string = reinterpret_cast<MonoString*>(aObject);
    MonoError error;

    auto valueAsBytes = mono_string_to_utf8_checked(string, &error);
    std::string value{ valueAsBytes };
    mono_free(valueAsBytes);

    return Any{ value };
  }

  template <>
  Any MonoUnboxRedirect<String>(MonoObject *aObject)
  {
    auto string = reinterpret_cast<MonoString*>(aObject);
    MonoError error;

    auto valueAsBytes = mono_string_to_utf8_checked(string, &error);
    String value{ valueAsBytes };
    mono_free(valueAsBytes);

    return Any{ value };
  }

  void Assembly::FilterTypes(std::vector<std::string> aTypes)
  {
    mTypes.clear();

    for (auto &classRef : aTypes)
    {
      auto splitClass = split(classRef, ',', false);

      if (splitClass[0] == mName)
      {
        mTypes.emplace_back(splitClass[4], splitClass[5]);
      }
    }
  }

  CSharpRedirect::CSharpRedirect(DocumentedObject *aObject, 
                                 MonoClass *aClass, 
                                 std::string_view aPrimitiveName,
                                 ObjectToAny aUnboxer/*,
                                 AnyToObject aBoxer*/)
    : mUnbox(aUnboxer)
    //, mBox(aBoxer)
    , mClass(aClass)
    , mType(static_cast<Type*>(aObject))
    , mInitializeFunction(nullptr)
    , mMonoName(mono_class_get_name(aClass))
    , mMonoPrimitiveName(aPrimitiveName)
  {
    YTEUnusedArgument(aObject);
  }


  CSharpFunction::CSharpFunction(DocumentedObject *aObject, 
                                 Function *aFunction, 
                                 MonoMethod *aMethod,
                                 Type *aType)
    : mFunction(aFunction)
    , mMethod(aMethod)
    , mType(aType)
  {

  }


  YTEDefineType(CSharpSystem)
  {
    YTERegisterType(CSharpSystem);
  }
 
  CSharpSystem::CSharpSystem(Composition *aOwner, RSValue *aProperties) 
    : Component(aOwner, nullptr) 
  { 
    YTEUnusedArgument(aProperties);
    aOwner->YTERegister(Events::LogicUpdate, this, &CSharpSystem::Update);
  } 
 
  CSharpSystem::~CSharpSystem() 
  { 
 
  }

  // Not redirected yet.
  //MONO_API MonoClass* mono_get_object_class(void);
  //MONO_API MonoClass* mono_get_intptr_class(void);
  //MONO_API MonoClass* mono_get_uintptr_class(void);
  //MONO_API MonoClass* mono_get_enum_class(void);
  //MONO_API MonoClass* mono_get_array_class(void);
  //MONO_API MonoClass* mono_get_thread_class(void);
  //MONO_API MonoClass* mono_get_exception_class(void);

  void CSharpSystem::SetUpRedirect()
  {
    TypeId<String>()->AddAttribute<CSharpRedirect>(mono_get_string_class(), "string", MonoUnboxRedirect<String>);
    TypeId<std::string>()->AddAttribute<CSharpRedirect>(mono_get_string_class(), "string", MonoUnboxRedirect<std::string>);

    TypeId<byte>()->AddAttribute<CSharpRedirect>(mono_get_byte_class(), "byte", MonoUnboxRedirect<byte>);
    TypeId<char>()->AddAttribute<CSharpRedirect>(mono_get_char_class(), "string", MonoUnboxRedirect<char>);

    TypeId<bool>()->AddAttribute<CSharpRedirect>(mono_get_boolean_class(), "bool", MonoUnboxRedirect<bool>);
    TypeId<void>()->AddAttribute<CSharpRedirect>(mono_get_void_class(), "void", nullptr);

    TypeId<u8>()->AddAttribute<CSharpRedirect>(mono_get_byte_class(), "byte", MonoUnboxRedirect<u8>);
    TypeId<u16>()->AddAttribute<CSharpRedirect>(mono_get_uint16_class(), "ushort", MonoUnboxRedirect<u16>);
    TypeId<u32>()->AddAttribute<CSharpRedirect>(mono_get_uint32_class(), "uint", MonoUnboxRedirect<u32>);
    TypeId<u64>()->AddAttribute<CSharpRedirect>(mono_get_uint64_class(), "ulong", MonoUnboxRedirect<u64>);
    TypeId<i8>()->AddAttribute<CSharpRedirect>(mono_get_sbyte_class(), "sbyte", MonoUnboxRedirect<i8>);
    TypeId<i16>()->AddAttribute<CSharpRedirect>(mono_get_int16_class(), "short", MonoUnboxRedirect<i16>);
    TypeId<i32>()->AddAttribute<CSharpRedirect>(mono_get_int32_class(), "int", MonoUnboxRedirect<i32>);
    TypeId<i64>()->AddAttribute<CSharpRedirect>(mono_get_int64_class(), "long", MonoUnboxRedirect<i64>);

    TypeId<float>()->AddAttribute<CSharpRedirect>(mono_get_single_class(), "float", MonoUnboxRedirect<float>);
    TypeId<double>()->AddAttribute<CSharpRedirect>(mono_get_double_class(), "double", MonoUnboxRedirect<double>);
  }

  static void SetVTableFields(Type *aType, MonoClass *aClass)
  {
    std::vector<std::pair<std::string, Function*>> functions;
    std::vector<std::pair<std::string, Function*>> properties;
    std::vector<std::pair<std::string, Function*>> fields;

    for (auto &functionIt : aType->GetFunctions())
    {
      auto function = functionIt.second.get();
      auto name = CSharpCodeGenerator::GenerateVTableName(function);
      functions.emplace_back(name, function);
    }

    for (auto &propertyIt : aType->GetProperties())
    {
      auto property = propertyIt.second.get();

      if (property->GetGetter())
      {
        auto getterName = CSharpCodeGenerator::GenerateVTableName(property, true);
        properties.emplace_back(getterName, property->GetGetter());
      }

      if (property->GetSetter())
      {
        auto setterName = CSharpCodeGenerator::GenerateVTableName(property, false);
        properties.emplace_back(setterName, property->GetSetter());
      }
    }

    for (auto &fieldIt : aType->GetFields())
    {
      auto field = static_cast<Field*>(fieldIt.second.get());

      if (field->GetGetter())
      {
        auto getterName = CSharpCodeGenerator::GenerateVTableName(field, true);
        fields.emplace_back(getterName, field->GetGetter());
      }

      if (field->GetSetter())
      {
        auto setterName = CSharpCodeGenerator::GenerateVTableName(field, false);
        fields.emplace_back(setterName, field->GetSetter());
      }
    }

    auto setVTablePtr = [aClass](std::pair<std::string, Function*> &aPair)
    {
      std::string getFnName{ "GetVTablePointer_" };
      getFnName += aPair.first;
      std::string setFnName{ "SetVTablePointer_" };
      setFnName += aPair.first;

      auto getFnMethod = mono_class_get_method_from_name(aClass, getFnName.c_str(), -1);
      runtime_assert(nullptr != getFnMethod, "Can't find the getter method.");

      auto setFnMethod = mono_class_get_method_from_name(aClass, setFnName.c_str(), -1);
      runtime_assert(nullptr != setFnMethod, "Can't find the setter method.");

      using Getter = void*(__stdcall *)(MonoException**);
      using Setter = void(__stdcall *)(void*, MonoException**);

      MonoException *except;
      auto getFn = (Getter)mono_method_get_unmanaged_thunk(getFnMethod);
      runtime_assert(nullptr != getFn, "Can't find the getter method thunk.");

      auto setFn = (Setter)mono_method_get_unmanaged_thunk(setFnMethod);
      runtime_assert(nullptr != setFn, "Can't find the setter method thunk.");

      auto before = getFn(&except);
      setFn(static_cast<void*>(aPair.second), &except);
      auto after = getFn(&except);
    };

    std::for_each(functions.begin(), functions.end(), setVTablePtr);
    std::for_each(properties.begin(), properties.end(), setVTablePtr);
    std::for_each(fields.begin(), fields.end(), setVTablePtr);
  }

  template <typename tType>
  tType& MonoArrayGet(MonoArray *aArray, size_t aIndex)
  {
    auto it = static_cast<void*>(mono_array_addr_with_size(aArray, sizeof(tType), aIndex));
    return *static_cast<tType*>(it);
  }

  template <typename tType>
  tType& MonoArraySet(tType &aValue, MonoArray *aArray, size_t aIndex)
  {
    auto item = MonoArrayGet<tType>(aArray, aIndex);
    *item = aValue;
  }

  extern "C"
  {
    MonoObject* MonoInternalFunctionCall(u64 aFunction, MonoArray *aArray)
    {
      Function *fn = reinterpret_cast<Function*>(aFunction);

      auto parameters = fn->GetParameters();

      auto length = mono_array_length(aArray);

      std::vector<Any> anys;

      for (size_t i = 0; i < length; ++i)
      {
        auto type = parameters[i].mType->GetMostBasicType();
        auto redirect = type->GetAttribute<CSharpRedirect>();

        auto object = MonoArrayGet<MonoObject*>(aArray, i);

        if (redirect)
        {
          anys.emplace_back(redirect->Unbox(object));
        }
        else
        {
          auto item = *MonoUnbox<u64>(object);

          anys.emplace_back(item, type, true);
        }
      }

      auto toReturn = fn->Invoke(anys);

      auto returnType = fn->GetReturnType();

      return nullptr;
    }

    void MonoInternalFunctionVoidCall(u64 aFunction, MonoArray *aArray)
    {
      Function *fn = reinterpret_cast<Function*>(aFunction);

      auto csharpFunction = fn->GetAttribute<CSharpFunction>();
      auto unboxers = csharpFunction->GetParameterUnboxers();

      auto length = mono_array_length(aArray);

      std::vector<Any> anys;

      for (size_t i = 0; i < length; ++i)
      {
        auto object = MonoArrayGet<MonoObject*>(aArray, i);

        if (nullptr != unboxers[i])
        {
          anys.emplace_back(unboxers[i](object));
        }
        else
        {
          auto item = *MonoUnbox<u64>(object);

          anys.emplace_back(item, csharpFunction->GetReturnType() , true);
        }
      }

      fn->Invoke(anys);
    }

    MonoObject* MonoInternalPropertyGetCall(u64 aGetter, u64 aThisPointer)
    {
      Function *getterFn = reinterpret_cast<Function*>(aGetter);
      YTEUnusedArgument(aThisPointer);

      return nullptr;
    }

    void MonoInternalPropertySetCall(u64 aSetter, u64 aThisPointer, u64 aValue)
    {
      Function *setterFn = reinterpret_cast<Function*>(aSetter);
      YTEUnusedArgument(aThisPointer);
      YTEUnusedArgument(aValue);
    }
  }

  std::vector<std::string> GetAllTypes(MonoMethod *aGetTypes)
  {
    auto classesObj = mono_runtime_invoke(aGetTypes, nullptr, nullptr, nullptr);
    auto classes = reinterpret_cast<MonoArray*>(classesObj);

    auto length = mono_array_length(classes);

    std::vector<std::string> classesVec;

    for (size_t i = 0; i < length; ++i)
    {
      auto string = MonoArrayGet<MonoString*>(classes, i);
      MonoError error;

      auto classAsBytes = mono_string_to_utf8_checked(string, &error);
      classesVec.emplace_back(classAsBytes);
      mono_free(classAsBytes);

    }

    return std::move(classesVec);
  }

  Assembly& CSharpSystem::AddAssembly(std::string_view aFile)
  {
    fs::path csharpBinPath = Path::GetCSharpBinaryFolder(Path::GetGamePath());

#if defined(NDEBUG)
    csharpBinPath /= "Release";
#else
    csharpBinPath /= "Debug";
#endif

    std::string fileName{ aFile };
    fs::path path = csharpBinPath / fileName;
    path.concat(".dll");

    auto fileAsString = path.string();

    auto assembly = mono_domain_assembly_open(mRootDomain, fileAsString.c_str());
    auto image = mono_assembly_get_image(assembly);

    if (nullptr == image)
    {
      printf("C# assembly could not be opened.\n");
    }

    Assembly toEmplace{ assembly, aFile };
    auto emplaced = mAssemblies.emplace(fileName, toEmplace);

    return emplaced.first->second;
  }
 
  void CSharpSystem::Initialize() 
  { 
    mono_config_parse(NULL);
    
    mRootDomain = mono_jit_init_version("YTEEngineRoot", "v4.0.30319");

    SetUpRedirect();

    auto &yteAssembly = AddAssembly("YTE");

    //auto image = mono_assembly_get_image(yteAssembly.mMonoAssembly);
    //auto asmClass = mono_class_from_name(image, "YTE", "YTEInternalCall");
    //mGetClasses = mono_class_get_method_from_name(asmClass, "GetClasses", 0);
    //
    //auto &gameProjectAssembly = AddAssembly("GameProject");
    //
    //auto allTypes = GetAllTypes(mGetClasses);
    //
    //yteAssembly.FilterTypes(allTypes);
    //gameProjectAssembly.FilterTypes(allTypes);

    //for (auto &typeIt : Type::GetGlobalTypes())
    //{
    //  if (typeIt.second->GetAttribute<CSharpRedirect>())
    //  {
    //    continue;
    //  }
    //
    //  auto typeName = CSharpCodeGenerator::ParseTypeName(typeIt.second->GetMostBasicType());
    //  typeName += "_vtable";
    //
    //  std::string_view typeView{ typeName };
    //
    //  // We default to YTE as many types don't include it in the name.
    //  std::string namespaceToLook{ "YTE" };
    //
    //  auto it = typeView.find_last_of(".");
    //
    //  if (it != std::string_view::npos)
    //  {
    //    namespaceToLook += '.';
    //    namespaceToLook += typeView.substr(0, it);
    //    typeView = typeView.substr(it + 1);
    //  }
    //
    //  std::string justType{ typeView };
    //
    //  auto monoClass = mono_class_from_name(yteImage, namespaceToLook.c_str(), justType.c_str());
    //
    //  if (nullptr != monoClass)
    //  {
    //    SetVTableFields(typeIt.second, monoClass);
    //  }
    //  else
    //  {
    //    runtime_assert(false, "We're forgetting to set a CSharp VTable.");
    //  }
    //}
  } 


  constexpr const char *projectBeginText = 
R"DELIMITER(<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <Import Project="$(MSBuildExtensionsPath)\$(MSBuildToolsVersion)\Microsoft.Common.props" Condition="Exists('$(MSBuildExtensionsPath)\$(MSBuildToolsVersion)\Microsoft.Common.props')" />
  <PropertyGroup>
    <Configuration Condition=" '$(Configuration)' == '' ">Debug</Configuration>
    <Platform Condition=" '$(Platform)' == '' ">AnyCPU</Platform>
)DELIMITER";

  // Then this
  //<ProjectGuid>{THIS_IS_A_PROJECT_GUID}</ProjectGuid>

  constexpr const char *projectMiddleText =
R"DELIMITER(<TargetFrameworkVersion>v4.6.1</TargetFrameworkVersion>
    <FileAlignment>512</FileAlignment>
    <AutoGenerateBindingRedirects>true</AutoGenerateBindingRedirects>
  </PropertyGroup>
  <PropertyGroup Condition=" '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' ">
    <PlatformTarget>AnyCPU</PlatformTarget>
    <DebugSymbols>true</DebugSymbols>
    <DebugType>full</DebugType>
    <Optimize>false</Optimize>
    <OutputPath>../../CSharpBinary/Debug</OutputPath>
    <DefineConstants>DEBUG;TRACE</DefineConstants>
    <ErrorReport>prompt</ErrorReport>
    <WarningLevel>4</WarningLevel>
    <AllowUnsafeBlocks>true</AllowUnsafeBlocks> 
  </PropertyGroup>
  <PropertyGroup Condition=" '$(Configuration)|$(Platform)' == 'Release|AnyCPU' ">
    <PlatformTarget>AnyCPU</PlatformTarget>
    <DebugType>pdbonly</DebugType>
    <Optimize>true</Optimize>
    <OutputPath>../../CSharpBinary/Release</OutputPath>
    <DefineConstants>TRACE</DefineConstants>
    <ErrorReport>prompt</ErrorReport>
    <WarningLevel>4</WarningLevel>
    <AllowUnsafeBlocks>true</AllowUnsafeBlocks> 
  </PropertyGroup>
  <PropertyGroup>
    <StartupObject />
  </PropertyGroup>
  <ItemGroup>
)DELIMITER";

  constexpr const char *projectMiddle2Text =
R"DELIMITER(  </ItemGroup>
  <ItemGroup>
)DELIMITER";

  // Then these types of lines:
  // <Compile Include="FILE_NAME.cs" />

  constexpr const char *projectEndText = 
R"DELIMITER(</ItemGroup>
  <ItemGroup>
    <None Include="App.config" />
  </ItemGroup>
  <Import Project="$(MSBuildToolsPath)\Microsoft.CSharp.targets" />
</Project>
)DELIMITER";


  constexpr const char *appConfigText = 
R"DELIMITER(<?xml version="1.0" encoding="utf-8" ?>
<configuration>
    <startup> 
        <supportedRuntime version="v4.0" sku=".NETFramework,Version=v4.6.1" />
    </startup>
</configuration>
)DELIMITER";


  constexpr const char *slnBeginText = 
R"DELIMITER(Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio 15
VisualStudioVersion = 15.0.26730.15
MinimumVisualStudioVersion = 10.0.40219.1
)DELIMITER";


  // Then these types of lines:
  //Project("{THIS_IS_A_SOLUTION_GUID}") = "PROJECT_NAME", "RELATIVE_PROJECT_PATH.csproj", "{THIS_IS_A_PROJECT_GUID}"
  //EndProject
  
  constexpr const char *slnEndText = 
R"DELIMITER(Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|Any CPU = Debug|Any CPU
		Release|Any CPU = Release|Any CPU
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{3B18C4D3-59EE-4BDA-BA8B-F29CFC8816FA}.Debug|Any CPU.ActiveCfg = Debug|Any CPU
		{3B18C4D3-59EE-4BDA-BA8B-F29CFC8816FA}.Debug|Any CPU.Build.0 = Debug|Any CPU
		{3B18C4D3-59EE-4BDA-BA8B-F29CFC8816FA}.Release|Any CPU.ActiveCfg = Release|Any CPU
		{3B18C4D3-59EE-4BDA-BA8B-F29CFC8816FA}.Release|Any CPU.Build.0 = Release|Any CPU
		{B4F3093D-69B4-4967-96CB-4A64A036B527}.Debug|Any CPU.ActiveCfg = Debug|Any CPU
		{B4F3093D-69B4-4967-96CB-4A64A036B527}.Debug|Any CPU.Build.0 = Debug|Any CPU
		{B4F3093D-69B4-4967-96CB-4A64A036B527}.Release|Any CPU.ActiveCfg = Release|Any CPU
		{B4F3093D-69B4-4967-96CB-4A64A036B527}.Release|Any CPU.Build.0 = Release|Any CPU
	EndGlobalSection
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
	GlobalSection(ExtensibilityGlobals) = postSolution
		SolutionGuid = {E304E880-2DEB-4C0F-88ED-BFB1D9117208}
	EndGlobalSection
EndGlobal

)DELIMITER";


  constexpr const char *getTypesFunction =
    R"DELIMITER(
public static string[] GetClasses()
{
  var classes = new List<string>();
  var componentType = typeof(Component);

  foreach (var assembly in System.AppDomain.CurrentDomain.GetAssemblies())
  {
    foreach (var type in assembly.GetTypes())
    {
      if (type.IsSubclassOf(componentType))
      {
        var builder = new StringBuilder();
        builder.Append(assembly.GetName());
        builder.Append(",");
        builder.Append(type.Namespace);
        builder.Append(",");
        builder.Append(type.Name);

        classes.Add(builder.ToString());
      }
    }
  }

  return classes.ToArray();
}
)DELIMITER";


  std::string CSharpSystem::GenerateYTECode()
  {
    CSharpCodeGenerator codeGenerator;
    
    codeGenerator.AddLine("using System.Collections.Generic;");
    codeGenerator.AddLine("using System.Text;");
    codeGenerator.AddLine("using System.Runtime.CompilerServices;");
    codeGenerator.AddLine("");

    codeGenerator.OpenNamespace("YTE");

    codeGenerator.AddClass("YTEInternalCall", nullptr);

    codeGenerator.AddLine(getTypesFunction);

    codeGenerator.AddLine("[MethodImpl(MethodImplOptions.InternalCall)]");
    codeGenerator.AddLine("public static extern void functionCall_void_internal(System.UInt64 aFunctionPointer, object[] aArray);");
    codeGenerator.AddLine("[MethodImpl(MethodImplOptions.InternalCall)]");
    codeGenerator.AddLine("public static extern object functionCall_internal(System.UInt64 aFunctionPointer, object[] aArray);");
    codeGenerator.AddLine("[MethodImpl(MethodImplOptions.InternalCall)]");
    codeGenerator.AddLine("public static extern object getCall_internal(System.UInt64 aFunctionPointer, System.UInt64 aThis);");
    codeGenerator.AddLine("[MethodImpl(MethodImplOptions.InternalCall)]");
    codeGenerator.AddLine("public static extern void setCall_internal(System.UInt64 aFunctionPointer, System.UInt64 aThis, object aSetter);");
    codeGenerator.AddLine("[MethodImpl(MethodImplOptions.InternalCall)]");
    codeGenerator.AddLine("public static extern System.UInt64 allocateType(System.UInt64 aSize);");
    codeGenerator.AddLine("[MethodImpl(MethodImplOptions.InternalCall)]");
    codeGenerator.AddLine("public static extern void deallocateType(System.UInt64 aThis);");

    codeGenerator.RemoveScope();

    auto wwise = mOwner->GetComponent<WWiseSystem>();
    auto banks = wwise->GetBanks();

    for (auto &i : banks)
    {
      codeGenerator.OpenNamespace(i.second.mName); // Open Bank

      if (i.second.mEvents.size())
      {
        std::vector<std::pair<std::string_view, u64>> enums;

        for (auto &event : i.second.mEvents)
        {
          enums.emplace_back(event.mName, event.mId);
        }

        codeGenerator.AddEnums("Events", enums);
      }

      if (i.second.mRTPCs.size())
      {
        std::vector<std::pair<std::string_view, u64>> rtpcs;

        for (auto &rtpc : i.second.mRTPCs)
        {
          rtpcs.emplace_back(rtpc.mName, rtpc.mId);
        }

        codeGenerator.AddEnums("RTPCs", rtpcs);
      }

      if (i.second.mSwitchGroups.size())
      {
        std::vector<std::pair<std::string_view, u64>> switchGroups;

        for (auto &switchGroupList : i.second.mStateGroups)
        {
          auto &stateGroup = switchGroupList.second.first;
          switchGroups.emplace_back(stateGroup.mName, stateGroup.mId);

          std::vector<std::pair<std::string_view, u64>> switches;

          for (auto &aSwitch : switchGroupList.second.second)
          {
            switches.emplace_back(aSwitch.mName, aSwitch.mId);
          }

          codeGenerator.AddEnums(stateGroup.mName, switches);
        }

        codeGenerator.AddEnums("StateGroups", switchGroups);
      }

      if (i.second.mStateGroups.size())
      {
        std::vector<std::pair<std::string_view, u64>> stateGroups;

        for (auto &stateGroupList : i.second.mStateGroups)
        {
          auto &stateGroup = stateGroupList.second.first;
          stateGroups.emplace_back(stateGroup.mName, stateGroup.mId);

          std::vector<std::pair<std::string_view, u64>> states;

          for (auto &state : stateGroupList.second.second)
          {
            states.emplace_back(state.mName, state.mId);
          }

          codeGenerator.AddEnums(stateGroup.mName, states);
        }

        codeGenerator.AddEnums("StateGroups", stateGroups);
      }

      codeGenerator.CloseNamespace(); // close Bank
    }

    for (auto &type : Type::GetGlobalTypes())
    {
      codeGenerator.AddType(type.second);
    }

    codeGenerator.CloseNamespace(); // close YTE

    return codeGenerator.ToString();
  }

  static void WriteUTF8File(fs::path &aFilePath, std::string_view aFileText)
  {
    std::ofstream log;
    log.open(aFilePath, std::ios::out | std::ios::trunc | std::ios::binary);

    //log << 0xEF;
    //log << 0xBB;
    //log << 0xBF;

    log << aFileText << '\n';

    log.close();
  }
  
  std::string CSharpSystem::GenerateProjectFile(std::string_view aProjectName, 
                                                GlobalUniqueIdentifier aProjectGuid,
                                                std::vector<std::string_view> &aFiles,
                                                std::vector<std::string_view> &aReferences)
  {
    std::string toReturn{ projectBeginText };

    toReturn += "<ProjectGuid>";
    toReturn += aProjectGuid.ToString();
    toReturn += "</ProjectGuid>";


    toReturn += "\n<OutputType>Library</OutputType>\n";
    toReturn += "<RootNamespace>";
    toReturn += aProjectName;
    toReturn +=  "</RootNamespace>\n";

    toReturn += "<AssemblyName>";
    toReturn += aProjectName;
    toReturn += "</AssemblyName>\n";

    toReturn += projectMiddleText;

    for (auto &reference : aReferences)
    {
      toReturn += "  <Reference Include=\"";
      toReturn += reference;
      toReturn += "\" />";
    }

    toReturn += projectMiddle2Text;

    for (auto &file : aFiles)
    {
      toReturn += "  <Compile Include=\"";
      toReturn += file;
      toReturn += ".cs";
      toReturn += "\" />";
    }

    toReturn += projectEndText;

    return toReturn;
  }

  void CSharpSystem::GenerateSolution()
  {
    fs::path csharpPath = Path::GetCSharpFolder(Path::GetGamePath());

    if (false == fs::exists(csharpPath))
    {
      std::error_code code;
      fs::create_directories(csharpPath, code);
    }

    fs::path csharpBinPath = Path::GetCSharpBinaryFolder(Path::GetGamePath());

    if (false == fs::exists(csharpBinPath))
    {
      std::error_code code;
      fs::create_directories(csharpBinPath, code);
    }

    GlobalUniqueIdentifier slnGuid;
    GlobalUniqueIdentifier yteProjGuid;

    std::string appConfig{ appConfigText };

    auto yteCode = GenerateYTECode();
    std::vector<std::string_view> yteFiles{ "YTE" };
    std::vector<std::string_view> yteReferences{ "System"};
    auto yteProjText = GenerateProjectFile("YTE", yteProjGuid, yteFiles, { yteReferences });

    std::vector<std::string_view> userFiles{ "MyTransform" };
    std::vector<std::string_view> userReferences{ "System", "YTE" };
    GlobalUniqueIdentifier userProjGuid;
    auto userProjText = GenerateProjectFile("GameProject", userProjGuid, userFiles, userReferences);

    std::string slnText{ slnBeginText };
    slnText += "EndProject\n";

    // YTE Project
    slnText += "Project(\"";
    slnText += slnGuid.ToString();
    slnText += "\") = \"YTE\", \"YTE/YTE.csproj\", \"";
    slnText += yteProjGuid.ToString();
    slnText += "\"\nEndProject\n";
    
    // User Project
    slnText += "Project(\"";
    slnText += slnGuid.ToString();
    slnText += "\") = \"GameProject\", \"GameProject/GameProject.csproj\", \"";
    slnText += userProjGuid.ToString();
    slnText += "\"\nEndProject\n";

    slnText += slnEndText;

    auto slnPath = csharpPath / "GameProject.sln";
    WriteUTF8File(slnPath, slnText);

    auto ytePath = csharpPath / "YTE";
    if (false == fs::exists(ytePath))
    {
      std::error_code code;
      fs::create_directories(ytePath, code);
    }

    auto userPath = csharpPath / "GameProject";
    if (false == fs::exists(userPath))
    {
      std::error_code code;
      fs::create_directories(userPath, code);
    }

    auto yteCodePath = ytePath / "YTE.cs";
    WriteUTF8File(yteCodePath, yteCode);
    auto yteProjPath = ytePath / "YTE.csproj";
    WriteUTF8File(yteProjPath, yteProjText);
    auto yteAppConfigPath = ytePath / "App.config";
    WriteUTF8File(yteAppConfigPath, appConfig);

    auto userProjPath = userPath / "GameProject.csproj";
    WriteUTF8File(userProjPath, userProjText);
    auto userAppConfigPath = userPath / "App.config";
    WriteUTF8File(userAppConfigPath, appConfig);

    for (auto &file : userFiles)
    {
      std::string fileStr{ file };
      auto userFilePath = userPath / fileStr;
      userFilePath += ".cs";

      if (false == fs::exists(userFilePath))
      {
        CSharpCodeGenerator codeGenerator;
        codeGenerator.OpenNamespace("GameProject");
        codeGenerator.AddClass(file, Component::GetStaticType(), false);
        codeGenerator.RemoveScope();
        codeGenerator.CloseNamespace();

        auto code = codeGenerator.ToString();

        WriteUTF8File(userFilePath, code);
      }
    }
  }
 
  void CSharpSystem::Update(LogicUpdate *aEvent) 
  {
    YTEUnusedArgument(aEvent);
  } 
}
