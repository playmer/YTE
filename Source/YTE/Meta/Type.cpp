
#include "YTE/Meta/Meta.hpp"
#include "YTE/Meta/Type.hpp"

#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  std::unordered_map<std::string, Type*> Type::sGlobalTypes;

  YTEDefineType(DocumentedObject)
  {
    RegisterType<DocumentedObject>();
    TypeBuilder<DocumentedObject> builder;
    builder.Property<&DocumentedObject::GetDocumentation, &DocumentedObject::SetDocumentation>("Documentation");
  }

  YTEDefineType(Type)
  {
    RegisterType<Type>();
    TypeBuilder<Type> builder;

    builder.Function<&Type::GetGlobalType>("GetGlobalType")
      .SetParameterNames("aName");

    builder.Property<&Type::Name, NoSetter>("Name")
      .SetDocumentation("Name of the Type.");
    builder.Property<&Type::Hash, NoSetter>("Hash")
      .SetDocumentation("Hash of the Type.");
    builder.Property<&Type::GetAllocatedSize, NoSetter>("AllocatedSize")
      .SetDocumentation("Allocated size of the Type.");
    builder.Property<&Type::GetStoredSize, NoSetter>("StoredSize")
      .SetDocumentation("Stored size of the Type.");
    builder.Property<&Type::GetUnqualifiedSize, NoSetter>("UnqualifiedSize")
      .SetDocumentation("Unqualified size of the Type.");
  }

  Type::~Type()
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
      printf("Could not find a type named %s, did you rename/misspell it/forget to Define/InitializeType it?\n", aName.c_str());
    }

    return toReturn;
  }

  YTEDefineType(Property)
  {
    RegisterType<Property>();
    TypeBuilder<Property> builder;

    builder.Property<&Property::GetOwningType, NoSetter>("OwningType")
      .SetDocumentation("Type that owns this Property.");
    builder.Property<&Property::GetPropertyType, NoSetter>("PropertyType")
      .SetDocumentation("Type of the Property, what we can get or set.");
    builder.Property<&Property::GetName, NoSetter>("Name")
      .SetDocumentation("Name of the Property.");
    builder.Property<&Property::GetGetter, NoSetter>("Getter")
      .SetDocumentation("Getter function of the Property, may be null.");
    builder.Property<&Property::GetSetter, NoSetter>("Setter")
      .SetDocumentation("Setter function of the Property, may be null.");
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
    TypeBuilder<Field> builder;
  }

  YTEDefineType(Any)
  {
    RegisterType<Any>();
    TypeBuilder<Any> builder;
  }

  void SerializeYteString(void* aAnyToSerialize, void* aValue, void* aAllocator)
  {
    auto any = static_cast<Any*>(aAnyToSerialize);
    auto value = static_cast<RSValue*>(aValue);
    auto allocator = static_cast<RSAllocator*>(aAllocator);

    auto& returnValue = any->As<String>();
    value->SetString(returnValue.c_str(), static_cast<RSSizeType>(returnValue.Size()), *allocator);
  }

  void SerializeStdString(void* aAnyToSerialize, void* aValue, void* aAllocator)
  {
    auto any = static_cast<Any*>(aAnyToSerialize);
    auto value = static_cast<RSValue*>(aValue);
    auto allocator = static_cast<RSAllocator*>(aAllocator);

    auto& returnValue = any->As<std::string>();
    value->SetString(returnValue.c_str(), static_cast<RSSizeType>(returnValue.size()), *allocator);
  }
}

YTEDefineExternalType(YTE::String)
{
  RegisterType<YTE::String>();
  TypeBuilder<YTE::String> builder;

  builder.Function<&YTE::SerializeYteString>("JsonSerialize");
}

YTEDefineExternalType(std::string)
{
  RegisterType<std::string>();
  TypeBuilder<std::string> builder;

  builder.Function<&YTE::SerializeStdString>("JsonSerialize");
}

YTEDefineExternalType(YTE::s8)
{
  RegisterType<YTE::s8>();
  TypeBuilder<YTE::s8> builder;
}

YTEDefineExternalType(YTE::i8)
{
  RegisterType<YTE::i8>();
  TypeBuilder<YTE::i8> builder;
}

YTEDefineExternalType(YTE::i16)
{
  RegisterType<YTE::i16>();
  TypeBuilder<YTE::i16> builder;
}

YTEDefineExternalType(YTE::i32)
{
  RegisterType<YTE::i32>();
  TypeBuilder<YTE::i32> builder;
}

YTEDefineExternalType(YTE::i64)
{
  RegisterType<YTE::i64>();
  TypeBuilder<YTE::i64> builder;
}

YTEDefineExternalType(YTE::u8)
{
  RegisterType<YTE::u8>();
  TypeBuilder<YTE::u8> builder;
}

YTEDefineExternalType(YTE::u16)
{
  RegisterType<YTE::u16>();
  TypeBuilder<YTE::u16> builder;
}

YTEDefineExternalType(YTE::u32)
{
  RegisterType<YTE::u32>();
  TypeBuilder<YTE::u32> builder;
}

YTEDefineExternalType(YTE::u64)
{
  RegisterType<YTE::u64>();
  TypeBuilder<YTE::u64> builder;
}

YTEDefineExternalType(void)
{
  RegisterType<void>();
  TypeBuilder<void> builder;
}

YTEDefineExternalType(bool)
{
  RegisterType<bool>();
  TypeBuilder<bool> builder;
}

YTEDefineExternalType(float)
{
  RegisterType<float>();
  TypeBuilder<float> builder;
}

YTEDefineExternalType(double)
{
  RegisterType<double>();
  TypeBuilder<double> builder;
}
