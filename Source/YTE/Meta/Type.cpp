
#include "YTE/Meta/Meta.hpp"
#include "YTE/Meta/Type.hpp"

namespace YTE
{
  std::unordered_map<std::string, Type*> Type::sGlobalTypes;

  YTEDefineType(DocumentedObject)
  {
    RegisterType<DocumentedObject>();
    YTEBindProperty(&DocumentedObject::GetDocumentation, &DocumentedObject::SetDocumentation, "Documentation");
  }

  YTEDefineType(Type)
  {
    RegisterType<Type>();

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

  YTEDefineType(Property)
  {
    RegisterType<Property>();

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

    // TODO: This check doesn't work as intended, should be something more like, convertible to.
    //if (mSetter && mGetter)
    //{
    //  auto parameters = mSetter->GetParameters();
    //
    //  DebugObjection(parameters[1].mType->GetMostBasicType() != mGetter->GetReturnType()->GetMostBasicType(),
    //                 "%s %s must have a setter that takes as it's "
    //                 "first parameter, the same type as it's getter returns. \n"
    //                 "  Setter First Parameter Type: %s"
    //                 "  Getter Return Type : %s",
    //                 mOwningType->GetName().c_str(),
    //                 mName.c_str(),
    //                 parameters[1].mType->GetName().c_str(),
    //                 mGetter->GetName().c_str());
    //}
  }

  YTEDefineType(Field)
  {
    RegisterType<Field>();
  }

  YTEDefineExternalType(YTE::String)
  {
    RegisterType<YTE::String>();
  }
}


YTEDefineExternalType(YTE::s8)
{
  RegisterType<YTE::s8>();
}

YTEDefineExternalType(YTE::i8)
{
  RegisterType<YTE::i8>();
}

YTEDefineExternalType(YTE::i16)
{
  RegisterType<YTE::i16>();
}

YTEDefineExternalType(YTE::i32)
{
  RegisterType<YTE::i32>();
}

YTEDefineExternalType(YTE::i64)
{
  RegisterType<YTE::i64>();
}

YTEDefineExternalType(YTE::u8)
{
  RegisterType<YTE::u8>();
}

YTEDefineExternalType(YTE::u16)
{
  RegisterType<YTE::u16>();
}

YTEDefineExternalType(YTE::u32)
{
  RegisterType<YTE::u32>();
}

YTEDefineExternalType(YTE::u64)
{
  RegisterType<YTE::u64>();
}

YTEDefineExternalType(void)
{
  RegisterType<void>();
}

YTEDefineExternalType(bool)
{
  RegisterType<bool>();
}

YTEDefineExternalType(float)
{
  RegisterType<float>();
}

YTEDefineExternalType(double)
{
  RegisterType<double>();
}

YTEDefineExternalType(std::string)
{
  RegisterType<std::string>();
}
