#pragma once

#include "YTE/Meta/Property.hpp"
#include "YTE/StandardLibrary/Any.hpp"
#include "YTE/StandardLibrary/TypeTraits.hpp"

namespace YTE
{
  class Field : public Property
  {
  public:
    YTEDeclareType(Field)
      Field(Field&) = delete;

    Field(const char *aName,
          std::unique_ptr<Function> aGetter,
          std::unique_ptr<Function> aSetter)
      : Property(aName, std::move(aGetter), std::move(aSetter))
    {
    }

    template<typename FieldPointerType, FieldPointerType aFieldPointer>
    static size_t GetOffset()
    {
      using ObjectType = typename DecomposeFieldPointer<FieldPointerType>::ObjectType;
      using FieldType = typename DecomposeFieldPointer<FieldPointerType>::FieldType;
      std::array<char, sizeof(ObjectType)> selfData;
      auto selfVoid = static_cast<void*>(selfData.data());
      ObjectType *self{ static_cast<ObjectType*>(selfVoid) };

      auto bytePtr = reinterpret_cast<byte*>(&(self->*aFieldPointer));
      byte *byteAtSelf{ static_cast<byte*>(selfVoid) };
      
      return bytePtr - byteAtSelf;
    }

    template<typename FieldPointerType, FieldPointerType aFieldPointer>
    static Any Getter(std::vector<Any>& aArguments)
    {
      auto self = aArguments.at(0).As<typename DecomposeFieldPointer<FieldPointerType>::ObjectType*>();
      return Any(self->*aFieldPointer);
    }

    template<typename FieldPointerType, FieldPointerType aFieldPointer>
    static Any Setter(std::vector<Any>& aArguments)
    {
      auto self = aArguments.at(0).As<typename DecomposeFieldPointer<FieldPointerType>::ObjectType*>();
      self->*aFieldPointer = aArguments.at(1).As<typename DecomposeFieldPointer<FieldPointerType>::FieldType>();
      return Any();
    }

    void SetOffset(size_t aOffset)
    {
      mOffset = aOffset;
    }

    size_t GetOffset() { return mOffset; }

  private:
    size_t mOffset;
  };
}