#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "YTE/StandardLibrary/OrderedMultiMap.hpp"
#include "YTE/StandardLibrary/OrderedMap.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"
#include "YTE/StandardLibrary/TypeTraits.hpp"

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

  template <typename tType>
  typename tType::SelfType GetSelfType(typename tType::SelfType*) {}

  template <typename tType>
  void GetSelfType(...) {}


  template <typename tType>
  tType GetDummy(void (tType::*)());



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

  template<typename tType>
  inline Type* TypeId();


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

    template <typename tType, typename... tArguments>
    DocumentedObject& AddAttribute(tArguments &&...aArguments)
    {
      auto attribute = std::make_unique<tType>(this, std::forward<tArguments &&>(aArguments)...);
      mAttributes.Emplace(TypeId<tType>(), std::move(attribute));

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
    explicit Type(char const* aName, tDerived *, tBase *)
      : mName(aName)
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tDerived>())
      , mStoredSize(SizeOf<tDerived>())
      , mUnqualifiedSize(SizeOf<StripQualifiersT<tDerived>>())
      , mDefaultConstructor(GenericDefaultConstruct<StripSingleQualifierT<tDerived>>)
      , mCopyConstructor(GenericCopyConstruct<StripSingleQualifierT<tDerived>>)
      , mMoveConstructor(GenericMoveConstruct<StripSingleQualifierT<tDerived>>)
      , mDestructor(GenericDestruct<tDerived>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(TypeId<tBase>())
    {
    }

    template <typename tType>
    explicit Type(char const* aName, tType *)
      : mName(aName)
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tType>())
      , mStoredSize(SizeOf<tType>())
      , mUnqualifiedSize(SizeOf<StripQualifiersT<tType>>())
      , mDefaultConstructor(GenericDefaultConstruct<StripSingleQualifierT<tType>>)
      , mCopyConstructor(GenericCopyConstruct<StripSingleQualifierT<tType>>)
      , mMoveConstructor(GenericMoveConstruct<StripSingleQualifierT<tType>>)
      , mDestructor(GenericDestruct<tType>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(nullptr)
    {
    }
    
    
    template <typename tDerived, typename tBase>
    explicit Type(tDerived*, tBase*)
      : mName(GetTypeName<tDerived>().data())
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tDerived>())
      , mStoredSize(SizeOf<tDerived>())
      , mUnqualifiedSize(SizeOf<StripQualifiersT<tDerived>>())
      , mDefaultConstructor(GenericDefaultConstruct<StripSingleQualifierT<tDerived>>)
      , mCopyConstructor(GenericCopyConstruct<StripSingleQualifierT<tDerived>>)
      , mMoveConstructor(GenericMoveConstruct<StripSingleQualifierT<tDerived>>)
      , mDestructor(GenericDestruct<tDerived>)
      , mReferenceTo(nullptr)
      , mPointerTo(nullptr)
      , mConstOf(nullptr)
      , mBaseType(TypeId<tBase>())
    {
    }

    template <typename tType>
    explicit Type(Type* aType, Modifier aModifier, tType*)
      : mName(GetTypeName<tType>().data())
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tType>())
      , mStoredSize(SizeOf<tType>())
      , mUnqualifiedSize(SizeOf<StripQualifiersT<tType>>())
      , mDefaultConstructor(GenericDefaultConstruct<StripSingleQualifierT<tType>>)
      , mCopyConstructor(GenericCopyConstruct<StripSingleQualifierT<tType>>)
      , mMoveConstructor(GenericMoveConstruct<StripSingleQualifierT<tType>>)
      , mDestructor(GenericDestruct<tType>)
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

    template <typename tType>
    explicit Type(Type* aType, Modifier aModifier, tType*, bool)
      : mName(GetTypeName<tType&>().data())
      , mHash(std::hash<std::string>{}(mName))
      , mAllocatedSize(SizeOf<tType*>())
      , mStoredSize(SizeOf<tType*>())
      , mUnqualifiedSize(SizeOf<StripQualifiersT<tType*>>())
      , mDefaultConstructor(GenericDefaultConstruct<tType*>)
      , mCopyConstructor(GenericCopyConstruct<tType*>)
      , mMoveConstructor(GenericMoveConstruct<tType*>)
      , mDestructor(GenericDestruct<tType*>)
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

    Type* mReferenceTo;
    Type* mPointerTo;
    Type* mConstOf;
    Type* mBaseType;
    Type* mEnumOf;

    static std::unordered_map<std::string, Type*> sGlobalTypes;
  };


  template<typename tType>
  inline Type* TypeId();


  template<typename tType>
  inline void InitializeType();

  template<typename tType>
  struct TypeIdentification
  {
    static inline Type* TypeId()
    {
      return tType::GetStaticType();
    }

    static inline void InitializeType()
    {
      tType::InitializeType();
    }
  };


  template<>
  struct TypeIdentification<std::nullptr_t>
  {
    static inline Type* TypeId()
    {
      return nullptr;
    }
  };

  template<typename tType>
  struct TypeIdentification<tType*>
  {
    static inline Type* TypeId()
    {
      // TODO (Josh): Check to see if this needs the bool passed in at the end of the constructor.
      static Type type{ ::YTE::TypeId<tType>(), 
                        Type::Modifier::Pointer, 
                        static_cast<tType**>(nullptr), true};

      return &type;
    }
  };

  template<typename tType>
  struct TypeIdentification<tType&>
  {
    static inline Type* TypeId()
    {
      static Type type{ ::YTE::TypeId<tType>(), 
                        Type::Modifier::Reference, 
                        static_cast<tType*>(nullptr)};

      return &type;
    }
  };


  template<typename tType>
  struct TypeIdentification<const tType>
  {
    static inline Type* TypeId()
    {
      static Type type{ ::YTE::TypeId<tType>(), 
                        Type::Modifier::Const, 
                        static_cast<const tType*>(nullptr)};

      return &type;
    }
  };


  template<typename tType>
  inline Type* TypeId()
  {
    return TypeIdentification<tType>::TypeId();
  }

  // Specialized so void returns nullptr;
  template<typename tType>
  inline Type* BaseTypeId()
  {
    return TypeIdentification<tType>::TypeId();
  }

  template<>
  inline Type* BaseTypeId<void>()
  {
    return nullptr;
  }


  template<typename tType>
  inline void InitializeType()
  {
    TypeIdentification<tType>::InitializeType();
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

// Must be used outside of a namespace, clang and MSVC seem to be fine inside of YTE
// but GCC will not allow it, and I tend to think they're right.
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
