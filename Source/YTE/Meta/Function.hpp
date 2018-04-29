#pragma once

#include <iostream>

#include "YTE/StandardLibrary/Any.hpp"

namespace YTE
{
  class Function : public DocumentedObject
  {
    public:
    YTEDeclareType(Function)
      Function(Function&) = delete;

    using CallingFunction = Any(*)(std::vector<Any>& arguments);

    struct Parameter
    {
      Parameter(Type *aType, const char *aName);

      Type *mType;
      std::string mName;
    };

    Function(const char *aName, Type *aReturnType, Type *aOwningType);
    Any Invoke(std::vector<Any> &aArguments) const;

    // Will return default constructed Any if the arguments fail.
    template <typename ...tArguments>
    Any Invoke(tArguments...aArguments) const
    {
      auto args = Any::FromVariadic<tArguments...>(aArguments...);

      return Invoke(args);
    }

    void AddParameter(Type *aType, const char *aName = "");
    void SetCaller(CallingFunction aCaller);
    const std::string& GetName() const;
    void SetOwningType(Type *aOwningType);
    Type* GetOwningType() const;
    bool IsSame(Function &aFunction) const;
    Type* GetReturnType() const;
    bool IsStaticOrFree();
    void SetParameterNames(std::initializer_list<const char *> aNames);
    std::vector<Parameter>& GetParameters();

    private:
    std::string mName;
    std::vector<Parameter> mParameters;
    Type *mReturnType;
    CallingFunction mCaller;
    Type *mOwningType;
    bool mStaticOrFree;
  };

  namespace Detail::Meta
  {
    template <typename T>
    inline void AddParameter(std::unique_ptr<Function>& aFunction)
    {
      aFunction->AddParameter(TypeId<T>());
    }

    template <typename... Rest> struct ParseArguments;

    template <>
    struct ParseArguments<>
    {
      inline static void Parse(std::unique_ptr<Function>&)
      {
      }
    };

    template <typename First, typename... Rest>
    struct ParseArguments<First, Rest...>
    {
      inline static void Parse(std::unique_ptr<Function>& aFunction)
      {
        AddParameter<First>(aFunction);
        ParseArguments<Rest...>::Parse(aFunction);
      }
    };

    template <typename tFunctionSignature>
    struct FunctionBinding
    {
      using CallingType = Any(*)(std::vector<Any>&);

      template <typename tReturn>
      struct FunctionCaller {};

      // Free/Static Member Function
      template <typename tReturn, typename... tArguments>
      struct FunctionCaller<tReturn(*)(tArguments...)>
      {
        inline static
          std::unique_ptr<Function> MakeFunction(const char *aName)
        {
          auto function = std::make_unique<Function>(aName,
                                                     TypeId<tReturn>(),
                                                     nullptr);

          Detail::Meta::ParseArguments<tArguments...>::Parse(function);

          return std::move(function);
        }

        template <tFunctionSignature tFunction>
        inline static Any Invoke(std::vector<Any>& aArguments)
        {
          size_t i = 0;

          // We get a warning for functions that don't have arguments and thus don't use these.
          YTEUnusedArgument(aArguments);
          YTEUnusedArgument(i);

          if constexpr(std::is_void_v<tReturn>)
          {
            tFunction(aArguments.at(i++).As<tArguments>()...);
            return Any{};
          }
          else
          {
            tReturn capture = tFunction(aArguments.at(i++).As<tArguments>()...);
            Any toReturn{ capture, TypeId<tReturn>(), false == std::is_reference_v<tReturn> };
            return toReturn;
          }
        }
      };

      // Member Function
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionCaller<tReturn(tObject::*)(tArguments...)>
      {
        inline static
          std::unique_ptr<Function> MakeFunction(const char *aName)
        {
          auto function = std::make_unique<Function>(aName,
                                                     TypeId<tReturn>(),
                                                     TypeId<tObject>());

          Detail::Meta::ParseArguments<tObject*, tArguments...>::Parse(function);

          return std::move(function);
        }

        template <tFunctionSignature tFunction>
        inline static Any Invoke(std::vector<Any>& aArguments)
        {
          auto self = aArguments.at(0).As<tObject*>();

          size_t i = 1;

          // We get a warning for functions that don't have arguments and thus don't use these.
          YTEUnusedArgument(aArguments);
          YTEUnusedArgument(i);

          if constexpr(std::is_void_v<tReturn>)
          {
            (self->*tFunction)(aArguments.at(i++).As<tArguments>()...);
            return Any{};
          }
          else
          {
            tReturn capture = (self->*tFunction)(aArguments.at(i++).As<tArguments>()...);
            Any toReturn{ capture, TypeId<tReturn>(), false == std::is_reference_v<tReturn> };
            return toReturn;
          }
        }
      };

      // Const Member Function
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionCaller<tReturn(tObject::*)(tArguments...) const>
      {
        inline static
          std::unique_ptr<Function> MakeFunction(const char *aName)
        {
          auto function = std::make_unique<Function>(aName,
                                                     TypeId<tReturn>(),
                                                     TypeId<tObject>());

          Detail::Meta::ParseArguments<tObject*, tArguments...>::Parse(function);

          return std::move(function);
        }

        template <tFunctionSignature tFunction>
        inline static Any Invoke(std::vector<Any>& aArguments)
        {
          auto self = aArguments.at(0).As<tObject*>();

          size_t i = 1;

          // We get a warning for functions that don't have arguments and thus don't use these.
          YTEUnusedArgument(aArguments);
          YTEUnusedArgument(i);

          if constexpr(std::is_void_v<tReturn>)
          {
            (self->*tFunction)(aArguments.at(i++).As<tArguments>()...);
            return Any{};
          }
          else
          {
            tReturn capture = (self->*tFunction)(aArguments.at(i++).As<tArguments>()...);
            Any toReturn{ capture, TypeId<tReturn>(), false == std::is_reference_v<tReturn> };
            return toReturn;
          }
        }
      };

      template <tFunctionSignature tFunction>
      static Any Caller(std::vector<Any>& aArguments)
      {
        return FunctionCaller<tFunctionSignature>::template Invoke<tFunction>(aArguments);
      }

      static std::unique_ptr<Function> BindPassedFunction(const char *aName,
                                                          CallingType aCaller)
      {
        auto function = FunctionCaller<tFunctionSignature>::MakeFunction(aName);

        function->SetCaller(aCaller);

        return std::move(function);
      }

      template <tFunctionSignature tFunction>
      static std::unique_ptr<Function> BindFunction(const char *aName)
      {
        auto function = FunctionCaller<tFunctionSignature>::MakeFunction(aName);

        function->SetCaller(Caller<tFunction>);

        return std::move(function);
      }
    };

    template <>
    struct FunctionBinding<nullptr_t>
    {
      template <nullptr_t BoundFunc>
      static std::unique_ptr<Function> BindFunction(const char *name)
      {
        YTEUnusedArgument(name);

        return std::unique_ptr<Function>();
      }
    };
  }

  template <typename FunctionSignature, FunctionSignature aBoundFunction, size_t aSize = 0>
  static Function& BindFunction(const char *name, Type *aType, std::initializer_list<const char *> aParameterNames)
  {
    static_assert(aSize == CountFunctionArguments<FunctionSignature>::Size() || aSize == 0,
                  "If passing names of function parameters you must pass either exactly as many names as there are arguments, or 0.");

    auto function = Detail::Meta::FunctionBinding<FunctionSignature>::template BindFunction<aBoundFunction>(name);
    function->SetParameterNames(aParameterNames);
    function->SetOwningType(aType);

    auto ptr = aType->AddFunction(std::move(function));

    return *ptr;
  }
}