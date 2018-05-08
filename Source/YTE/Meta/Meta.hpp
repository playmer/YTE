#pragma once

#include "YTE/StandardLibrary/TypeTraits.hpp"
#include "YTE/Meta/Type.hpp"
#include "YTE/StandardLibrary/Any.hpp"
#include "YTE/Meta/Function.hpp"
#include "YTE/Meta/Field.hpp"
#include "YTE/Meta/Property.hpp"
#include "YTE/Meta/Attribute.hpp"

namespace YTE
{
  template <typename tFunctionSignature, tFunctionSignature tBoundFunction>
  constexpr auto SelectOverload()
  {
    return tBoundFunction;
  }

  template <typename tType>
  class TypeBuilder
  {
    public:
    TypeBuilder()
      : mType{ TypeId<tType>() }
    {

    }

    Type* GetType()
    {
      return mType;
    }

    template <typename tFunctionSignature>
    class FunctionBuilder
    {
      public:
      FunctionBuilder(Function* aFunction)
        : mFunction{ aFunction }
      {

      }

      Function* GetFunction()
      {
        return mFunction;
      }

      template <typename... tArguments>
      FunctionBuilder& SetParameterNames(tArguments&& ...aNames)
      {
        constexpr size_t passedNames = sizeof...(aNames);
        constexpr size_t funcNames = CountFunctionArguments<tFunctionSignature>::Size();
        static_assert(0 != funcNames, "You needn't set the parameter names for this function, as there are no parameters.");

        static_assert(passedNames == funcNames,
                      "If passing names of function parameters you must pass either exactly as many names as there are arguments, or 0.");

        mFunction->SetParameterNames({ std::forward<tArguments>(aNames)... });

        return *this;
      }

      FunctionBuilder& SetDocumentation(char const* aString)
      {
        mFunction->SetDocumentation(aString);

        return *this;
      }

      template <typename tType, typename... tArguments>
      FunctionBuilder& AddAttribute(tArguments&& ...aArguments)
      {
        mFunction->AddAttribute<tType>(std::forward<tArguments>(aArguments)...);

        return *this;
      }

      private:
      Function * mFunction;
    };

    template <auto tBoundFunction>
    FunctionBuilder<decltype(tBoundFunction)> Function(char const* aName)
    {
      using FunctionSignature = decltype(tBoundFunction);

      auto function = Detail::Meta::FunctionBinding<FunctionSignature>:: template BindFunction<tBoundFunction>(aName);
      function->SetOwningType(TypeId<tType>());

      auto ptr = TypeId<tType>()->AddFunction(std::move(function));

      return FunctionBuilder<FunctionSignature>{ ptr };
    }

    template <auto tGetterFunction, auto tSetterFunction>
    YTE::Property& Property(char const* aName)
    {
      using GetterFunctionSignature = decltype(tGetterFunction);
      using SetterFunctionSignature = decltype(tSetterFunction);

      std::unique_ptr<YTE::Function> getter;
      std::unique_ptr<YTE::Function> setter;

      if constexpr (tGetterFunction)
      {
        getter = Detail::Meta::FunctionBinding<GetterFunctionSignature>:: template BindFunction<tGetterFunction>("Getter");
      }

      if constexpr (tSetterFunction)
      {
        setter = Detail::Meta::FunctionBinding<SetterFunctionSignature>:: template BindFunction<tSetterFunction>("Setter");
      }

      auto property = std::make_unique<YTE::Property>(aName, std::move(getter), std::move(setter));

      auto ptr = TypeId<tType>()->AddProperty(std::move(property));

      return *ptr;
    }

    template <auto tFieldPointer>
    YTE::Field& Field(char const *aName, PropertyBinding aBinding)
    {
      using FieldPointerType = decltype(tFieldPointer);
      using FieldType = typename DecomposeFieldPointer<FieldPointerType>::FieldType;

      std::unique_ptr<YTE::Function> getter;
      std::unique_ptr<YTE::Function> setter;

      if (PropertyBinding::Get == aBinding || PropertyBinding::GetSet == aBinding)
      {
        getter = Detail::Meta::FunctionBinding<FieldType(tType::*)()>::BindPassedFunction("Getter", Field::Getter<FieldPointerType, tFieldPointer>);
      }

      if (PropertyBinding::Set == aBinding || PropertyBinding::GetSet == aBinding)
      {
        setter = Detail::Meta::FunctionBinding<void(tType::*)(FieldType)>::BindPassedFunction("Setter", Field::Setter<FieldPointerType, tFieldPointer>);
      }

      auto field = std::make_unique<YTE::Field>(aName, std::move(getter), std::move(setter));

      auto type = TypeId<typename DecomposeFieldPointer<FieldPointerType>::FieldType>();

      field->SetPropertyType(type);
      field->SetOffset(YTE::Field::GetOffset<FieldPointerType, tFieldPointer>());

      auto ptr = TypeId<tType>()->AddField(std::move(field));
      return *ptr;
    }

    private:
    Type * mType;
  };

  template <typename tType>
  void RegisterType()
  {
    auto type = YTE::TypeId<tType>();
    Type::AddGlobalType(type->GetName(), type);

    TypeBuilder<tType> builder;
    builder.Function<&::YTE::TypeId<tType>>("GetStaticType");
  }

  inline constexpr nullptr_t NoGetter = nullptr;
  inline constexpr nullptr_t NoSetter = nullptr;
}