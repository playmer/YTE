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
    YTEDeclareType(Any);


    template <typename ...Arguments>
    static std::array<Any, sizeof...(Arguments)> FromVariadic(Arguments&&... aArguments);


    ////////////////////////////////////////////////////////////
    // User Facing Construction
    ////////////////////////////////////////////////////////////
    Any()
    {
      mType = nullptr;
      memset(mData, 0, sizeof(mData));
    }

    void ConstructFrom(void* aObject, Type* aType)
    {
      Clear();
      mType = aType;
      byte* data = AllocateData(aType->GetStoredSize());
      aType->GetCopyConstructor()(aObject, data);
    }

    template <typename tType>
    explicit Any(tType&& aValue)
    {
      mType = TypeId<tType>();
      byte* data = AllocateData(sizeof(tType));
      new (data) tType(aValue);
    }

    template <typename tType>
    explicit Any(tType const&& aValue)
    {
      mType = TypeId<tType>();
      byte* data = AllocateData(sizeof(tType));
      new (data) tType(aValue);
    }

    template <typename tType>
    explicit Any(tType& aValue)
    {
      mType = TypeId<tType>();
      byte* data = AllocateData(sizeof(tType));
      new (data) tType(aValue);
    }

    explicit Any(Any& aValue)
    {
      mType = TypeId<Any>();
      byte* data = AllocateData(sizeof(Any));
      new (data) Any(static_cast<Any const&>(aValue));
    }

    template <typename tType>
    explicit Any(tType const& aValue)
    {
      mType = TypeId<tType>();
      byte* data = AllocateData(sizeof(tType));
      new (data) tType(aValue);
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

    //Any(void* object, Type* type, bool)
    //{
    //  mType = type;
    //  byte* data = AllocateData(type->GetStoredSize());
    //  type->GetCopyConstructor()(object, data);
    //}

    ////////////////////////////////////////////////////////////
    // Moves
    ////////////////////////////////////////////////////////////
    Any(Any&& aRight)
      : mType(nullptr)
    {
      (*this) = aRight;
    }

    Any& operator=(Any&& aRight)
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

    ////////////////////////////////////////////////////////////
    // Copies
    ////////////////////////////////////////////////////////////
    Any(Any const& aRight)
      : mType(nullptr)
    {
      (*this) = aRight;
    }

    Any& operator=(Any const& aRight)
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

  template <typename ...tArguments>
  std::array<Any, sizeof...(tArguments)> Any::FromVariadic(tArguments&&... aArguments)
  {
    std::array<Any, sizeof...(tArguments)> arguments{std::forward<tArguments>(aArguments)...};

    return arguments;
  }
}