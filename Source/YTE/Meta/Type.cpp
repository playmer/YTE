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

    YTEBindStaticOrFreeFunction(Type, &Type::GetGlobalType, YTENoOverload, "GetGlobalType", YTEParameterNames("aName"));

    YTEBindProperty(&Type::Name, YTENoSetter, "Name")
      .Description() = "Name of the Type.";
    YTEBindProperty(&Type::Hash, YTENoSetter, "Hash")
      .Description() = "Hash of the Type.";
    YTEBindProperty(&Type::GetAllocatedSize, YTENoSetter, "AllocatedSize")
      .Description() = "Allocated size of the Type.";
    YTEBindProperty(&Type::GetStoredSize, YTENoSetter, "StoredSize")
      .Description() = "Stored size of the Type.";
    YTEBindProperty(&Type::GetUnqualifiedSize, YTENoSetter, "UnqualifiedSize")
      .Description() = "Unqualified size of the Type.";
  }

  inline Type::~Type()
  {

  }

  Function* Type::AddFunction(std::unique_ptr<Function> aFunction)
  {
    auto function = mFunctions.Emplace(aFunction->GetName(), std::move(aFunction));
    return function->second.get();
  }

  Property* Type::AddProperty(std::unique_ptr<Property> aProperty)
  {
    auto property = mProperties.Emplace(aProperty->GetName(), std::move(aProperty));
    return property->second.get();
  }

  Field* Type::AddField(std::unique_ptr<Field> aField)
  {
    auto field = mFields.Emplace(aField->GetName(), std::move(aField));
    return static_cast<Field*>(field->second.get());
  }

  bool Type::IsA(Type *aType, Type *aTypeToStopAt)
  {
    DebugAssert(IsA(aTypeToStopAt),
                "The aTypeToStopAt must be a base type of the type being searched.");

    Type *base = this;

    while (base && base != aTypeToStopAt)
    {
      if (base == aType)
      {
        return true;
      }

      base = base->GetBaseType();
    }

    return false;
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

    while (type->GetPointerTo()   ||
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

    YTEBindProperty(&Function::GetOwningType, YTENoSetter, "OwningType")
      .Description() = "Type that owns this Function.";
    YTEBindProperty(&Function::GetName, YTENoSetter, "Name")
      .Description() = "Name of the property.";
    YTEBindProperty(&Function::GetReturnType, YTENoSetter, "ReturnType")
      .Description() = "The return type of the function.";
    YTEBindProperty(&Function::IsStaticOrFree, YTENoSetter, "StaticOrFree")
      .Description() = "Lets you know if this function is a static or free function, as in not a member function.";
  }


  YTEDefineType(Property)
  {
    YTERegisterType(Property);

    YTEBindProperty(&Property::GetOwningType, YTENoSetter, "OwningType")
      .Description() = "Type that owns this Property.";
    YTEBindProperty(&Property::GetPropertyType, YTENoSetter, "PropertyType")
      .Description() = "Type of the Property, what we can get or set.";
    YTEBindProperty(&Property::GetName, YTENoSetter, "Name")
      .Description() = "Name of the Property.";
    YTEBindProperty(&Property::GetGetter, YTENoSetter, "Getter")
      .Description() = "Getter function of the Property, may be null.";
    YTEBindProperty(&Property::GetSetter, YTENoSetter, "Setter")
      .Description() = "Setter function of the Property, may be null.";
  }

  Property::Property(const char *aName,
                     std::unique_ptr<Function> aGetter,
                     std::unique_ptr<Function> aSetter)
    : mName(aName)
    , mGetter(std::move(aGetter))
    , mSetter(std::move(aSetter))
  {
    DebugAssert((nullptr != mGetter) || (nullptr != mSetter),
                "At least one of the getter and setter must be set.");

    if (mGetter)
    {
      mOwningType = mGetter->GetOwningType();
      mType = mGetter->GetReturnType()->GetMostBasicType();
    }

    if (mSetter)
    {
      mOwningType = mSetter->GetOwningType();
      auto parameters = mSetter->GetParameters();

      DebugObjection(parameters.size() != 2,
                     "%s %s must have a setter that takes only one parameter,"
                     " an instance of an object of the same type as it's getter returns.",
                     mOwningType->GetName().c_str(),
                     mName.c_str());

      mType = parameters[1].mType->GetMostBasicType();
    }

    if (mSetter && mGetter)
    {
      auto parameters = mSetter->GetParameters();

      DebugObjection(parameters[1].mType->GetMostBasicType() != mGetter->GetReturnType()->GetMostBasicType(),
                     "%s %s must have a setter that takes as it's "
                     "first parameter, the same type as it's getter returns. \n"
                     "  Setter First Parameter Type: %s"
                     "  Getter Return Type : %s",
                     mOwningType->GetName().c_str(),
                     mName.c_str(),
                     parameters[1].mType->GetName().c_str(),
                     mGetter->GetName().c_str());
    }
  }

  YTEDefineType(Field)
  {
    YTERegisterType(Field);
  }

  YTEDefineExternalType(YTE::String)
  {
    YTERegisterType(YTE::String);
  }
}


YTEDefineExternalType(YTE::s8)
{
  YTERegisterType(YTE::s8);
}

YTEDefineExternalType(YTE::i8)
{
  YTERegisterType(YTE::i8);
}

YTEDefineExternalType(YTE::i16)
{
  YTERegisterType(YTE::i16);
}

YTEDefineExternalType(YTE::i32)
{
  YTERegisterType(YTE::i32);
}

YTEDefineExternalType(YTE::i64)
{
  YTERegisterType(YTE::i64);
}

YTEDefineExternalType(YTE::u8)
{
  YTERegisterType(YTE::u8);
}

YTEDefineExternalType(YTE::u16)
{
  YTERegisterType(YTE::u16);
}

YTEDefineExternalType(YTE::u32)
{
  YTERegisterType(YTE::u32);
}

YTEDefineExternalType(YTE::u64)
{
  YTERegisterType(YTE::u64);
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
