#include "YTE/Meta/Type.hpp"

namespace YTE
{
  std::unordered_map<std::string, Type*> Type::sGlobalTypes;

  YTEDefineType(DocumentedObject)
  {
    YTERegisterType(DocumentedObject);
    YTEBindProperty(&DocumentedObject::GetDocumentation, &DocumentedObject::SetDocumentation, "Documentation");
  }

  YTEDefineType(Type)
  {
    YTERegisterType(Type);
  }

  inline Type::~Type()
  {

  }

  void Type::AddFunction(std::unique_ptr<Function> aFunction)
  {
    mFunctions.Emplace(aFunction->GetName(), std::move(aFunction));
  }

  void Type::AddProperty(std::unique_ptr<Property> aProperty)
  {
    mProperties.Emplace(aProperty->GetName(), std::move(aProperty));
  }

  void Type::AddField(std::unique_ptr<Field> aField)
  {
    mFields.Emplace(aField->GetName(), std::move(aField));
  }


  bool Type::IsA(Type *aType)
  {
    Type *base = this;

    if (base == aType)
    {
      return true;
    }

    while (base->GetBaseType())
    {
      base = base->GetBaseType();

      if (base == aType)
      {
        return true;
      }
    }

    return false;
  }

  Type* Type::GetMostBasicType()
  {
    Type *type = this;

    while (type->GetPointerTo() ||
      type->GetReferenceTo() ||
      type->GetConstOf())
    {
      if (type->GetPointerTo())
      {
        type = type->GetPointerTo();
      }
      else if (type->GetReferenceTo())
      {
        type = type->GetReferenceTo();
      }
      else if (type->GetConstOf())
      {
        type = type->GetConstOf();
      }
    }

    return type;
  }

  Property* Type::GetFirstField(const char *aName)
  {
    std::string name{ aName };

    auto it = mFields.FindFirst(name);

    if (it != mFields.end())
    {
      return it->second.get();
    }

    return nullptr;
  }

  void Type::AddGlobalType(const std::string &aName, Type *aType)
  {
    auto it = sGlobalTypes.find(aName);

    if (it != sGlobalTypes.end())
    {
      std::cout << "Type of the name " << aName << " already exists, not adding." << std::endl;
      return;
    }

    sGlobalTypes.emplace(aName, aType);
  }

  Type* Type::GetGlobalType(const std::string &aName)
  {
    auto it = sGlobalTypes.find(aName);

    Type *toReturn{ nullptr };

    if (it != sGlobalTypes.end())
    {
      toReturn = it->second;
    }

    if (toReturn == nullptr)
    {
      printf("Could not find a type named %s, did you rename/misspell it/forget to Define/InitializeType it?", aName.c_str());
    }

    return toReturn;
  }

  YTEDefineType(Function)
  {
    YTERegisterType(Function);

  }


  YTEDefineType(Property)
  {
    YTERegisterType(Property);

  }

  YTEDefineType(Field)
  {
    YTERegisterType(Field);
  }

  YTEDefineExternalType(YTE::String)
  {
    YTERegisterType(YTE::String);
  }


  YTEDefineExternalType(s8)
  {
    YTERegisterType(s8);
  }

  YTEDefineExternalType(i8)
  {
    YTERegisterType(i8);
  }

  YTEDefineExternalType(i16)
  {
    YTERegisterType(i16);
  }

  YTEDefineExternalType(i32)
  {
    YTERegisterType(i32);
  }

  YTEDefineExternalType(i64)
  {
    YTERegisterType(i64);
  }

  YTEDefineExternalType(u8)
  {
    YTERegisterType(u8);
  }

  YTEDefineExternalType(u16)
  {
    YTERegisterType(u16);
  }

  YTEDefineExternalType(u32)
  {
    YTERegisterType(u32);
  }

  YTEDefineExternalType(u64)
  {
    YTERegisterType(u64);
  }
}


YTEDefineExternalType(void)
{
  YTERegisterType(void);
}

YTEDefineExternalType(bool)
{
  YTERegisterType(bool);
}

YTEDefineExternalType(float)
{
  YTERegisterType(float);
}

YTEDefineExternalType(double)
{
  YTERegisterType(double);
}

YTEDefineExternalType(std::string)
{
  YTERegisterType(std::string);
}
