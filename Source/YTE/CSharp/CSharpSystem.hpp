/******************************************************************************/ 
/*! 
* \author Joshua T. Fisher 
* \date   2015-09-19 
* 
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved. 
*/ 
/******************************************************************************/ 
#pragma once 

#ifndef YTE_CSharpSystem_hpp 
#define YTE_CSharpSystem_hpp 

#include <mono/jit/jit.h> 

#include <unordered_map> 
#include <vector> 

#include "YTE/Core/Component.hpp" 

#include "YTE/CSharp/ForwardDeclarations.hpp"

#include "YTE/Meta/Type.hpp" 

#include "YTE/Utilities/Utilities.h"


namespace YTE 
{
  class CSharpRedirect : public Attribute
  {
  public:
    using ObjectToAny = Any(*)(MonoObject*);
    using AnyToObject = MonoObject*(*)(Any, CSharpRedirect*);
    
    using Constructor = void(__stdcall *)(MonoObject *, u64, bool, MonoException**);
    using Destrutor = void(__stdcall *)(MonoObject *, MonoException**);

    YTEDeclareType(CSharpRedirect);
    CSharpRedirect(DocumentedObject *aObject, 
                   MonoClass *aClass, 
                   std::string_view aPrimitiveName, 
                   ObjectToAny aUnboxer/*,
                   AnyToObject aBoxer*/);

    MonoClass* GetMonoClass()
    {
      return mClass;
    }

    std::string& MonoName()
    {
      return mMonoName;
    }

    std::string& MonoPrimitiveName()
    {
      return mMonoPrimitiveName;
    }

    Any Unbox(MonoObject *aObject)
    {
      return mUnbox(aObject);
    }

    MonoObject* Box(Any aAny)
    {
      return mBox(aAny, this);
    }


    Constructor GetConstructor() { return mConstructor; }
    Destrutor GetDestructor() { return mDestructor; }

  private:
    Constructor mConstructor;
    Destrutor mDestructor;
    ObjectToAny mUnbox;
    AnyToObject mBox;
    MonoClass *mClass;
    Type *mType;
    CSharpComponentInitFn mInitializeFunction;
    std::string mMonoName;
    std::string mMonoPrimitiveName;
  };

  class CSharpClass : public Attribute
  {
  public:
    YTEDeclareType(CSharpClass);

  private:
    MonoClass *mClass;
    MonoClassField *mNativeHandleField;
  };


  class CSharpFunction : public Attribute
  {
  public:
    YTEDeclareType(CSharpFunction);
    CSharpFunction(DocumentedObject *aObject, Function *aFunction, MonoMethod *aMethod, Type *aType);

    MonoMethod* GetMonoClass()
    {
      return mMethod;
    }

    std::vector<CSharpRedirect::ObjectToAny>& GetParameterUnboxers() 
    { 
      return mParametersToAnys;
    }

    CSharpRedirect::AnyToObject GetReturnBoxer() 
    { 
      return mReturnToMono; 
    }

    Type* GetReturnType()
    {
      return mType;
    }

  private:
    Function *mFunction;
    MonoMethod *mMethod;
    Type *mType;

    std::vector<CSharpRedirect::ObjectToAny> mParametersToAnys;
    CSharpRedirect::AnyToObject mReturnToMono;
  };

  class Assembly
  {
  public:
    Assembly(MonoAssembly *aMonoAssembly, 
             std::string_view aName)
      : mMonoAssembly(aMonoAssembly)
      , mName(aName)
    {
  
    }

    void FilterTypes(std::vector<std::string> aTypes);
  
    MonoAssembly *mMonoAssembly;
    std::string mName;
  
    // first = namespace, second = name
    std::vector<std::pair<std::string, std::string>> mTypes;
  };

  class CSharpSystem : public Component 
  { 
  public: 
    YTEDeclareType(CSharpSystem); 

    CSharpSystem(Composition *aOwner, RSValue *aProperties); 
    ~CSharpSystem() override; 

    Assembly& AddAssembly(std::string_view aFile);

    virtual void Initialize(); 
    void GenerateSolution();

    void SetUpRedirect();

    void Update(LogicUpdate *aEvent); 

  private:
   
    std::string GenerateYTECode();
    std::string GenerateProjectFile(std::string_view aProjectName,
                                    GlobalUniqueIdentifier aProjectGuid, 
                                    std::vector<std::string_view> &aFiles,
                                    std::vector<std::string_view> &aReferences);

    MonoDomain *mRootDomain;
    std::unordered_map<std::string, Assembly> mAssemblies;
    MonoMethod *mGetClasses;
  }; 
} 

#endif
