#pragma once

#include "YTE/Meta/Property.hpp"
#include "YTE/StandardLibrary/TypeTraits.hpp"

namespace YTE
{
  class Field : public Property
  {
  public:
    DeclareType(Field)
      Field(Field&) = delete;

    Field(const char *aName,
      std::unique_ptr<Function> aGetter,
      std::unique_ptr<Function> aSetter)
      : Property(aName, std::move(aGetter), std::move(aSetter))
    {
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

  private:
  };




  template<typename FieldPointerType, FieldPointerType aFieldPointer>
  static Field* BindField(const char *aName, PropertyBinding aBinding, Type *aType)
  {
    using ObjectType = typename DecomposeFieldPointer<FieldPointerType>::ObjectType;
    using FieldType = typename DecomposeFieldPointer<FieldPointerType>::FieldType;

    std::unique_ptr<Function> getter;
    std::unique_ptr<Function> setter;

    if (PropertyBinding::Get == aBinding || PropertyBinding::GetSet == aBinding)
    {
      getter = Binding<FieldType(ObjectType::*)()>::BindPassedFunction("Getter", Field::Getter<FieldPointerType, aFieldPointer>);
    }

    if (PropertyBinding::Set == aBinding || PropertyBinding::GetSet == aBinding)
    {
      setter = Binding<void(ObjectType::*)(FieldType)>::BindPassedFunction("Setter", Field::Setter<FieldPointerType, aFieldPointer>);
    }

    auto field = std::make_unique<Field>(aName, std::move(getter), std::move(setter));
    auto ptr = field.get();
    aType->AddField(std::move(field));
    return ptr;
  }

}