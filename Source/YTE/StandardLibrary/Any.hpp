#pragma once

#include "YTE/Meta/Type.hpp"
#include "YTE/Meta/ForwardDeclarations.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"
#include "YTE/StandardLibrary/TypeTraits.hpp"


namespace YTE
{
  class Any
  {
  public:

    template <typename ...Arguments>
    static std::vector<Any> FromVariadic(Arguments...aArguments);

    Any()
    {
      mType = nullptr;
      memset(mData, 0, sizeof(mData));
    }

    template<typename T>
    Any(const T& aObject, Type* aType, bool)
    {
      void *object = static_cast<void*>(const_cast<T*>(&aObject));
      mType = aType;
      auto size = aType->GetUnqualifiedSize();
      byte* data = AllocateData(size);

      aType->GetCopyConstructor()(object, data);
    }

    template<typename T>
    Any(const T* aObject, Type* aType, bool)
    {
      void *object = static_cast<void*>(const_cast<T*>(aObject));
      mType = aType;
      byte* data = AllocateData(aType->GetStoredSize());
      aType->GetCopyConstructor()(object, data);
    }

    Any(void* object, Type* type, bool)
    {
      mType = type;
      byte* data = AllocateData(type->GetStoredSize());
      type->GetCopyConstructor()(object, data);
    }


    Any& operator=(Any &&aRight)
    {
      Clear();

      mType = aRight.mType;

      if (nullptr != aRight.mType)
      {
        byte* data = AllocateData(mType->GetStoredSize());
        mType->GetMoveConstructor()(aRight.GetData(), data);
      }

      return (*this);
    }

    Any(Any &&aRight)
      : mType(nullptr)
    {
      (*this) = aRight;
    }

    Any& operator=(const Any &aRight)
    {
      Clear();

      mType = aRight.mType;

      if (nullptr != aRight.mType)
      {
        byte* data = AllocateData(mType->GetStoredSize());
        mType->GetCopyConstructor()(aRight.GetData(), data);
      }

      return (*this);
    }

    Any(const Any &aRight)
      : mType(nullptr)
    {
      (*this) = aRight;
    }

    void ConstructFrom(void* aObject, Type* aType)
    {
      Clear();
      mType = aType;
      byte* data = AllocateData(aType->GetStoredSize());
      aType->GetCopyConstructor()(aObject, data);
    }

    template <typename T>
    explicit Any(const T& value)
    {
      mType = TypeId<T>();
      byte* data = AllocateData(sizeof(T));
      new (data) T(value);
    }

    byte* AllocateData(size_t size)
    {
      if (size <= sizeof(mData))
      {
        return mData;
      }

      auto data = new byte[size];
      new (mData) byte*(data);
      return data;
    }

    const byte* GetData() const
    {
      if (mType->GetStoredSize() <= sizeof(mData))
      {
        return mData;
      }

      return *reinterpret_cast<byte* const *>(mData);
    }

    byte* GetData()
    {
      if (mType->GetStoredSize() <= sizeof(mData))
      {
        return mData;
      }

      return *reinterpret_cast<byte**>(mData);
    }

    void Clear()
    {
      if ((nullptr == mType) || (nullptr == mType->GetDestructor()))
      {
        return;
      }

      byte* data = GetData();
      mType->GetDestructor()(data);

      if (mType->GetStoredSize() > sizeof(mData))
      {
        delete data;
      }

      mType = nullptr;
      memset(mData, 0, sizeof(mData));
    }

    ~Any()
    {
      Clear();
    }

    template <typename T>
    bool IsType() const
    {
      auto typeId = TypeId<T>()->GetMostBasicType();
      auto thisType = mType->GetMostBasicType();
      auto first = typeId->IsA(thisType);
      auto second = thisType->IsA(typeId);
      return  first || second;
    }

    template <typename T>
    T& As()
    {
      runtime_assert(IsType<T>(), "This Any is being casted into the incorrect type.");

      return TypeCasting<T>::TypeCast(GetData());
    }

    Type* mType;
    byte mData[32];


  private:
    template<typename tTo>
    struct TypeCasting
    {
      static inline tTo& TypeCast(byte *aData)
      {
        return *reinterpret_cast<tTo*>(aData);
      }
    };

    template<typename tTo>
    struct TypeCasting<tTo&>
    {
      static inline tTo& TypeCast(byte *aData)
      {
        return *reinterpret_cast<tTo*>(aData);
      }
    };
  };





  template <typename... Rest> struct ParseAndAddArguments;

  template <>
  struct ParseAndAddArguments<>
  {
    inline static void Parse(std::vector<Any> &)
    {
    }
  };

  template <typename First, typename... Rest>
  struct ParseAndAddArguments<First, Rest...>
  {
    inline static void Parse(std::vector<Any> &aArguments, First aFirst, Rest ...aRest)
    {
      aArguments.emplace_back(aFirst);
      ParseAndAddArguments<Rest...>::Parse(aArguments, aRest...);
    }
  };

  template <typename ...Arguments>
  std::vector<Any> Any::FromVariadic(Arguments...aArguments)
  {
    std::vector<Any> arguments;
    ParseAndAddArguments<Arguments...>::Parse(arguments, aArguments...);

    return arguments;
  }
}