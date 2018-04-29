#pragma once

#include "YTE/Meta/Type.hpp"
#include "YTE/StandardLibrary/Any.hpp"
#include "YTE/Meta/Function.hpp"
#include "YTE/Meta/Field.hpp"
#include "YTE/Meta/Property.hpp"
#include "YTE/Meta/Attribute.hpp"

namespace YTE
{
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

    template <typename FunctionSignature>
    class FunctionBuilder
    {
      public:
      FunctionBuilder(Function *aFunction)
        : mFunction{ aFunction }
      {

      }

      Function* GetFunction()
      {
        return mFunction;
      }

      template <typename... tArguments>
      FunctionBuilder& SetParameterNames(tArguments &&...aNames)
      {
        constexpr size_t passedNames = sizeof...(aNames);
        constexpr size_t funcNames = CountFunctionArguments<FunctionSignature>::Size();
        static_assert(0 != funcNames, "You needn't set the parameter names for this function, as there are no parameters.");

        static_assert(passedNames == funcNames,
                      "If passing names of function parameters you must pass either exactly as many names as there are arguments, or 0.");

        mFunction->SetParameterNames({ std::forward<tArguments>(aNames)... });

        return *this;
      }

      FunctionBuilder& SetDocumentation(const char *aString)
      {
        mFunction->SetDocumentation(aString);

        return *this;
      }

      template <typename tType, typename... tArguments>
      FunctionBuilder& AddAttribute(tArguments &&...aArguments)
      {
        mFunction->AddAttribute<tType>(std::forward<tArguments>(aArguments)...);

        return *this;
      }

      private:
      Function * mFunction;
    };

    template <typename FunctionSignature, FunctionSignature aBoundFunction>
    FunctionBuilder<FunctionSignature> Function(const char *name)
    {
      auto function = Detail::Meta::FunctionBinding<FunctionSignature>:: template BindFunction<aBoundFunction>(name);
      function->SetOwningType(TypeId<tType>());

      auto ptr = TypeId<tType>()->AddFunction(std::move(function));

      return FunctionBuilder<FunctionSignature>{ ptr };
    }

    template <typename GetterFunctionSignature, GetterFunctionSignature tGetterFunction,
      typename SetterFunctionSignature, SetterFunctionSignature tSetterFunction>
      YTE::Property& Property(const char *aName)
    {
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

    template <typename FieldPointerType, FieldPointerType aFieldPointer>
    YTE::Field& Field(char const *aName, PropertyBinding aBinding)
    {
      using FieldType = typename DecomposeFieldPointer<FieldPointerType>::FieldType;

      std::unique_ptr<YTE::Function> getter;
      std::unique_ptr<YTE::Function> setter;

      if (PropertyBinding::Get == aBinding || PropertyBinding::GetSet == aBinding)
      {
        getter = Detail::Meta::FunctionBinding<FieldType(tType::*)()>::BindPassedFunction("Getter", Field::Getter<FieldPointerType, aFieldPointer>);
      }

      if (PropertyBinding::Set == aBinding || PropertyBinding::GetSet == aBinding)
      {
        setter = Detail::Meta::FunctionBinding<void(tType::*)(FieldType)>::BindPassedFunction("Setter", Field::Setter<FieldPointerType, aFieldPointer>);
      }

      auto field = std::make_unique<YTE::Field>(aName, std::move(getter), std::move(setter));

      auto type = TypeId<typename DecomposeFieldPointer<FieldPointerType>::FieldType>();

      field->SetPropertyType(type);
      field->SetOffset(YTE::Field::GetOffset<FieldPointerType, aFieldPointer>());

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
    builder.Function<decltype(&::YTE::TypeId<tType>), &::YTE::TypeId<tType>>("GetStaticType");
  }

  inline constexpr nullptr_t NoGetter = nullptr;
  inline constexpr nullptr_t NoSetter = nullptr;
}