#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "YTE/StandardLibrary/OrderedMultiMap.hpp"
#include "YTE/StandardLibrary/OrderedMap.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Meta/ForwardDeclarations.hpp"
#include "YTE/Meta/Reflection.hpp"

#include "YTE/Utilities/String/String.hpp"


namespace YTE
{
  class Base
  {
  public:
    virtual Type* GetType() = 0;
    virtual ~Base() {};
  };

  template <typename T>
  typename T::SelfType GetSelfType(typename T::SelfType*) {}

  template <typename T>
  void GetSelfType(...) {}


  template <typename T>
  T GetDummy(void (T::*)());



  // Used to declare a static type within a class
  // Requires YTEDefineType be used at some point in a
  // translation unit.
#define YTEDeclareType(Name)                                          \
void Dummy() {}                                                       \
typedef decltype(::YTE::GetDummy(&Name::Dummy)) TempSelfType;         \
typedef decltype(::YTE::GetSelfType<TempSelfType>(nullptr)) BaseType; \
typedef TempSelfType SelfType;                                        \
static ::YTE::Type sType;                                             \
static ::YTE::Type* GetStaticType() { return &sType; };               \
::YTE::Type* GetType() { return &sType; };                            \
static void InitializeType();


#define YTEDefineType(Name)                              \
::YTE::Type Name::sType{#Name,                                  \
                 static_cast<Name*>(nullptr),            \
                 static_cast<Name::BaseType*>(nullptr)}; \
void Name::InitializeType()

  enum class PropertyBinding
  {
    Get,
    Set,
    GetSet
  };


#define YTEParameterNames(...) {__VA_ARGS__}
#define YTENoNames {}
#define YTENoOverload
#define YTENoSetter nullptr
#define YTENoGetter nullptr

#define YTEBindFunction(aFunctionPointer, aOverloadResolution, aFunctionName, aInitializerListOfNames)  \
  ::YTE::BindFunction<decltype(aOverloadResolution aFunctionPointer),                                   \
               aFunctionPointer,                                                                        \
               std::initializer_list<const char*>aInitializerListOfNames.size()>(                       \
    aFunctionName,                                                                                      \
    ::YTE::TypeId<::YTE::DecomposeFunctionObjectType<decltype(aOverloadResolution aFunctionPointer)>::ObjectType>(), \
    aInitializerListOfNames)

#define YTEBindStaticOrFreeFunction(aType, aFunctionPointer, aOverloadResolution, aFunctionName, aInitializerListOfNames) \
  ::YTE::BindFunction<decltype(aOverloadResolution aFunctionPointer),                                                     \
                      aFunctionPointer,                                                                                   \
                      std::initializer_list<const char*>aInitializerListOfNames.size()>(aFunctionName,                    \
                                                                                        ::YTE::TypeId<aType>(),           \
                                                                                        aInitializerListOfNames)

  template<typename tEnumValueType, tEnumValueType tValue>
  tEnumValueType GetEnumAsNativeType()
  {
    return tValue;
  }


#define YTERegisterType(aType) \
  ::YTE::Type::AddGlobalType(::YTE::TypeId<aType>()->GetName(), ::YTE::TypeId<aType>()); \
  YTEBindStaticOrFreeFunction(aType, &::YTE::TypeId<aType>, YTENoOverload, "GetStaticType", YTENoNames)

#define YTEBindEnumValue(aEnumValue, aEnumName)    \
  ::YTE::BindFunction<decltype(GetEnumAsNativeType<typename std::underlying_type<decltype(aEnumValue)>::type, aEnumValue>),    \
                      GetEnumAsNativeType<typename std::underlying_type<decltype(aEnumValue)>::type, aEnumValue>,              \
                      std::initializer_list<const char*>YTENoNames.size()>(aEnumName,                                          \
                                                                           ::YTE::TypeId<decltype(aEnumValue)>(),              \
                                                                           YTENoNames)

#define YTEBindField(aFieldPointer, aName, aPropertyBinding)              \
  ::YTE::BindField<decltype(aFieldPointer), aFieldPointer>(               \
    aName,                                                                \
    aPropertyBinding,                                                     \
    ::YTE::TypeId<::YTE::DecomposeFieldPointer<decltype(aFieldPointer)>::ObjectType>())

#define YTEBindProperty(aGetterFunction, aSetterFunction, aName)            \
  ::YTE::BindProperty<decltype(aGetterFunction), aGetterFunction,           \
               decltype(aSetterFunction), aSetterFunction>(                 \
    aName,                                                                  \
    ::YTE::TypeId<::YTE::DecomposePropertyType<decltype(aGetterFunction),   \
                                 decltype(aSetterFunction)>::ObjectType>())


  class DocumentedObject : public Base
  {
  public:
    YTEDeclareType(DocumentedObject);

    DocumentedObject(const DocumentedObject &) = delete;

    DocumentedObject()
      : mDocumentation("")
    {

    }


    DocumentedObject(const char *aDocumentation)
      : mDocumentation(aDocumentation)
    {

    }

    const std::string& GetDocumentation()
    {
      return mDocumentation;
    }

    DocumentedObject& SetDocumentation(const char *aString)
    {
      mDocumentation = aString;
      return (*this);
    }

    std::string& Description()
    {
      return mDocumentation;
    }

    template <typename tType, typename... tArguments>
    DocumentedObject& AddAttribute(tArguments &&...aArguments)
    {
      mAttributes.Emplace(TypeId<tType>(), std::make_unique<tType>(this, std::forward<tArguments &&>(aArguments)...));

      return (*this);
    }

    template <typename tType>
    tType* GetAttribute()
    {
      auto it = mAttributes.Find(TypeId<tType>());
      tType *toReturn = nullptr;

      if (it != mAttributes.end())
      {
        toReturn = static_cast<tType*>(it->second.get());
      }

      return toReturn;
    }


  private:
    OrderedMap<Type*, std::unique_ptr<Attribute>> mAttributes;
    std::string mDocumentation;
  };

  class Type : public DocumentedObject
  {
  public:
    YTEDeclareType(Type)

    using DefaultConstructor = void(*)(void*);
    using CopyConstructor = void(*)(const void*, void*);
    using MoveConstructor = void(*)(void*, void*);
    using Destructor = void(*)(void*);

    enum class Modifier
    {
      Normal,
      Reference,
      Pointer,
      Const
    };

    template <typename tDerived, typename tBase>
    explicit Type(const char *aName, tDerived *, tBase *)
      : mName(aName)
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tDerived>())
      , mStoredSize(SizeOf<tDerived>())
      , mUnqualifiedSize(SizeOf<typename StripQualifiers<tDerived>::type>())
      , mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<tDerived>::type>)
      , mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<tDerived>::type>)
      , mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<tDerived>::type>)
      , mDestructor(GenericDestruct<tDerived>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(TypeId<tBase>())
    {
    }

    template <typename tType>
    explicit Type(const char *aName, tType *)
      : mName(aName)
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tType>())
      , mStoredSize(SizeOf<tType>())
      , mUnqualifiedSize(SizeOf<typename StripQualifiers<tType>::type>())
      , mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<tType>::type>)
      , mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<tType>::type>)
      , mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<tType>::type>)
      , mDestructor(GenericDestruct<tType>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(nullptr)
    {
    }
    
    
    template <typename tDerived, typename tBase>
    explicit Type(tDerived *, tBase *)
      : mName(GetTypeName<tDerived>().data())
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tDerived>())
      , mStoredSize(SizeOf<tDerived>())
      , mUnqualifiedSize(SizeOf<typename StripQualifiers<tDerived>::type>())
      , mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<tDerived>::type>)
      , mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<tDerived>::type>)
      , mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<tDerived>::type>)
      , mDestructor(GenericDestruct<tDerived>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(TypeId<tBase>())
    {
    }

    template <typename T>
    explicit Type(Type *aType, Modifier aModifier, T *)
      : mName(GetTypeName<T>().data())
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<T>())
      , mStoredSize(SizeOf<T>())
      , mUnqualifiedSize(typename SizeOf<StripQualifiers<T>::type>())
      , mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<T>::type>)
      , mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<T>::type>)
      , mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<T>::type>)
      , mDestructor(GenericDestruct<T>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(nullptr)
    {
      switch (aModifier)
      {
        case Modifier::Const:
        {
          mConstOf = aType;
          break;
        }
        case Modifier::Reference:
        {
          mReferenceTo = aType;
          break;
        }
        case Modifier::Pointer:
        {
          mPointerTo = aType;
          break;
        }
        case Modifier::Normal:
        default:
          break;
      }
    }

    template <typename T>
    explicit Type(Type *aType, Modifier aModifier, T*, bool)
      : mName(GetTypeName<T&>().data())
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<T*>())
      , mStoredSize(SizeOf<T*>())
      , mUnqualifiedSize(SizeOf<typename StripQualifiers<T *>::type>())
      , mDefaultConstructor(GenericDefaultConstruct<T*>)
      , mCopyConstructor(GenericCopyConstruct<T*>)
      , mMoveConstructor(GenericMoveConstruct<T*>)
      , mDestructor(GenericDestruct<T*>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(nullptr)
    {
      switch (aModifier)
      {
        case Modifier::Const:
        {
          mConstOf = aType;
          break;
        }
        case Modifier::Reference:
        {
          mReferenceTo = aType;
          break;
        }
        case Modifier::Pointer:
        {
          mPointerTo = aType;
          break;
        }
        case Modifier::Normal:
        default:
          break;
      }
    }

    Type(Type&) = delete;

    ~Type();

    const std::string& Name()  const { return mName; }
    size_t             Hash() const { return mHash; }
    size_t             GetAllocatedSize() const { return mAllocatedSize; }
    size_t             GetStoredSize() const { return mStoredSize; }
    size_t             GetUnqualifiedSize() const { return mUnqualifiedSize; }
    DefaultConstructor GetDefaultConstructor() const { return mDefaultConstructor; }
    CopyConstructor    GetCopyConstructor()  const { return mCopyConstructor; }
    MoveConstructor    GetMoveConstructor()  const { return mMoveConstructor; }
    Destructor         GetDestructor()  const { return mDestructor; }

    // Don't really need to do this, but w/e.
    bool operator==(const Type &aRight) const
    {
      if ((mHash == aRight.mHash) && (mName == aRight.mName))
      {
        return true;
      }

      return false;
    }

    Function* AddFunction(std::unique_ptr<Function> aFunction);
    Property* AddProperty(std::unique_ptr<Property> aProperty);
    Field* AddField(std::unique_ptr<Field>    aField);

    OrderedMultiMap<std::string, std::unique_ptr<Function>>::range GetFunctionRange(const std::string_view aName)
    {
      return mFunctions.FindAll(aName);
    }

    Function* GetFirstFunction(const std::string_view aName)
    {
      auto it = mFunctions.FindFirst(aName);

      if (it != mFunctions.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>::range GetPropertyRange(const char *aName)
    {
      std::string name{ aName };

      return mProperties.FindAll(name);
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>::range GetPropertyRange(const std::string &aName)
    {
      return mProperties.FindAll(aName);
    }

    Property* GetFirstProperty(const std::string &aName)
    {
      auto it = mProperties.FindFirst(aName);

      if (it != mProperties.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    Property* GetFirstProperty(const char *aName)
    {
      auto it = mProperties.FindFirst(aName);

      if (it != mProperties.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    Property* GetField(const std::string_view aName)
    {
      auto it = mFields.FindFirst(aName);

      if (it != mFields.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>::range GetFieldRange(const std::string &aName)
    {
      return mFields.FindAll(aName);
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>::range GetFieldRange(const char *aName)
    {
      std::string name{ aName };

      return mFields.FindAll(name);
    }

    bool IsA(Type *aType);
    bool IsA(Type *aType, Type *aTypeToStopAt);

    template<typename tType>
    bool IsA()
    {
      return IsA(TypeId<tType>());
    }

    Type* GetMostBasicType();
    Property* GetFirstField(const char *aName);

    Type* GetPointerTo()
    {
      return mPointerTo;
    }

    Type* GetReferenceTo()
    {
      return mReferenceTo;
    }

    Type* GetConstOf()
    {
      return mConstOf;
    }

    Type* GetEnumOf()
    {
      return mEnumOf;
    }

    void SetEnumOf(Type *aType)
    {
      mEnumOf = aType;
    }

    Type* GetBaseType()
    {
      return mBaseType;
    }

    const std::string& GetName() const
    {
      return mName;
    }

    static void AddGlobalType(const std::string &aName, Type *aType);
    static Type* GetGlobalType(const std::string &aName);

    static std::unordered_map<std::string, Type*>& GetGlobalTypes() { return sGlobalTypes; }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>& GetProperties() { return mProperties; };
    OrderedMultiMap<std::string, std::unique_ptr<Property>>& GetFields() { return mFields; };
    OrderedMultiMap<std::string, std::unique_ptr<Function>>& GetFunctions() { return mFunctions; };

  private:
    OrderedMultiMap<std::string, std::unique_ptr<Function>> mFunctions;
    OrderedMultiMap<std::string, std::unique_ptr<Property>> mProperties;
    OrderedMultiMap<std::string, std::unique_ptr<Property>> mFields;
    std::string mName;
    size_t mHash;
    size_t mAllocatedSize;
    size_t mStoredSize;
    size_t mUnqualifiedSize;
    DefaultConstructor mDefaultConstructor;
    CopyConstructor mCopyConstructor;
    MoveConstructor mMoveConstructor;
    Destructor mDestructor;

    Type *mReferenceTo;
    Type *mPointerTo;
    Type *mConstOf;
    Type *mBaseType;
    Type *mEnumOf;

    static std::unordered_map<std::string, Type*> sGlobalTypes;
  };


  template<typename T>
  inline Type* TypeId();


  template<typename T>
  inline void InitializeType();

  template<typename T>
  struct TypeIdentification
  {
    static inline Type* TypeId()
    {
      return T::GetStaticType();
    }

    static inline void InitializeType()
    {
      T::InitializeType();
    }
  };


  template<>
  struct TypeIdentification<nullptr_t>
  {
    static inline Type* TypeId()
    {
      return nullptr;
    }
  };

  template<typename T>
  struct TypeIdentification<T*>
  {
    static inline Type* TypeId()
    {
      // TODO (Austin): Check to see if this needs the bool passed in at the end of the constructor.
      static Type type{ ::YTE::TypeId<T>(), Type::Modifier::Pointer, static_cast<T**>(nullptr), true};

      return &type;
    }
  };

  template<typename T>
  struct TypeIdentification<T&>
  {
    static inline Type* TypeId()
    {
      static Type type{ ::YTE::TypeId<T>(), Type::Modifier::Reference, static_cast<T*>(nullptr)};

      return &type;
    }
  };


  template<typename T>
  struct TypeIdentification<const T>
  {
    static inline Type* TypeId()
    {
      static Type type{ ::YTE::TypeId<T>(), Type::Modifier::Const, static_cast<const T*>(nullptr)};

      return &type;
    }
  };


  template<typename T>
  inline Type* TypeId()
  {
    return TypeIdentification<T>::TypeId();
  }

  // Specialized so void returns nullptr;
  template<typename T>
  inline Type* BaseTypeId()
  {
    return TypeIdentification<T>::TypeId();
  }

  template<>
  inline Type* BaseTypeId<void>()
  {
    return nullptr;
  }


  template<typename T>
  inline void InitializeType()
  {
    TypeIdentification<T>::InitializeType();
  }
}

#define YTEDeclareExternalType(Name)                       \
namespace YTE                                              \
{                                                          \
  template<>                                               \
  struct TypeIdentification<Name>                          \
  {                                                        \
    static inline Type* TypeId()                           \
    {                                                      \
      static Type type{ #Name,                             \
                        static_cast<Name*>(nullptr) };     \
      return &type;                                        \
    }                                                      \
                                                           \
    static inline void InitializeType()                    \
    {                                                      \
      Type::AddGlobalType(TypeId()->GetName(), TypeId());  \
    }                                                      \
  };                                                       \
                                                           \
  template<>                                               \
  void InitializeType<Name>();                             \
}

#define YTEDefineExternalType(Name) template<> void YTE::InitializeType<Name>()

YTEDeclareExternalType(void)
YTEDeclareExternalType(bool)
YTEDeclareExternalType(YTE::s8)
YTEDeclareExternalType(YTE::i8)
YTEDeclareExternalType(YTE::i16)
YTEDeclareExternalType(YTE::i32)
YTEDeclareExternalType(YTE::i64)
YTEDeclareExternalType(YTE::u8)
YTEDeclareExternalType(YTE::u16)
YTEDeclareExternalType(YTE::u32)
YTEDeclareExternalType(YTE::u64)
YTEDeclareExternalType(float)
YTEDeclareExternalType(double)
YTEDeclareExternalType(std::string)
YTEDeclareExternalType(YTE::String)

#include "YTE/Meta/Function.hpp"
#include "YTE/Meta/Property.hpp"
#include "YTE/Meta/Field.hpp"
#include "YTE/Meta/Attribute.hpp"
