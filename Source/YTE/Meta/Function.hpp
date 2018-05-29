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

    YTE_Shared Function(const char *aName, Type *aReturnType, Type *aOwningType);
    YTE_Shared Any Invoke(std::vector<Any> &aArguments) const;

    // Will return default constructed Any if the arguments fail.
    template <typename ...tArguments>
    Any Invoke(tArguments...aArguments) const
    {
      auto args = Any::FromVariadic<tArguments...>(aArguments...);

      return Invoke(args);
    }

    YTE_Shared void AddParameter(Type *aType, const char *aName = "");
    YTE_Shared void SetCaller(CallingFunction aCaller);
    YTE_Shared const std::string& GetName() const;
    YTE_Shared void SetOwningType(Type *aOwningType);
    YTE_Shared Type* GetOwningType() const;
    YTE_Shared bool IsSame(Function &aFunction) const;
    YTE_Shared Type* GetReturnType() const;
    YTE_Shared bool IsStaticOrFree();
    YTE_Shared void SetParameterNames(std::initializer_list<const char *> aNames);
    YTE_Shared std::vector<Parameter>& GetParameters();

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

      template <typename Return, typename = void>
      struct FunctionInvoker {};
      
      /////////////////////////////////////////////
      // Free/Static Functions
      // Returns Something
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
        template <tFunctionSignature tFunction>
        inline static Any Invoke(std::vector<Any>& aArguments)
        {
          size_t i = 0;

          // We get a warning for functions that don't have arguments and thus don't use these.
          UnusedArguments(aArguments, i);

          tReturn capture = tFunction(aArguments.at(i++).As<tArguments>()...);
          Any toReturn{ capture, TypeId<tReturn>(), false == std::is_reference_v<tReturn> };
          return toReturn;
        }
      };

      // Void Return
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(tArguments...), EnableIf::IsVoid<tReturn>>
      {
        template <tFunctionSignature tFunction>
        inline static Any Invoke(std::vector<Any>& aArguments)
        {
          size_t i = 0;

          // We get a warning for functions that don't have arguments and thus don't use these.
          UnusedArguments(aArguments, i);

          tFunction(aArguments.at(i++).As<tArguments>()...);
          return Any{};
        }
      };


      /////////////////////////////////////////////
      // Free/Static Noexcept Function Pointers
      // Returns Something
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(tArguments...) noexcept, EnableIf::IsNotVoid<tReturn>>
        : public FunctionInvoker<tReturn(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
      };

      // Void Return
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(tArguments...) noexcept, EnableIf::IsVoid<tReturn>>
        : public FunctionInvoker<tReturn(tArguments...), EnableIf::IsVoid<tReturn>>
      {
      };


      /////////////////////////////////////////////
      // Free/Static Function Pointers
      // Returns Something
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(*)(tArguments...), EnableIf::IsNotVoid<tReturn>>
        : public FunctionInvoker<tReturn(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
      };

      // Void Return
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(*)(tArguments...), EnableIf::IsVoid<tReturn>>
        : public FunctionInvoker<tReturn(tArguments...), EnableIf::IsVoid<tReturn>>
      {
      };


      /////////////////////////////////////////////
      // Free/Static Noexcept Function Pointers
      // Returns Something
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(*)(tArguments...) noexcept, EnableIf::IsNotVoid<tReturn>>
        : public FunctionInvoker<tReturn(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
      };

      // Void Return
      template <typename tReturn, typename... tArguments>
      struct FunctionInvoker<tReturn(*)(tArguments...) noexcept, typename EnableIf::IsVoid<tReturn>>
        : public FunctionInvoker<tReturn(tArguments...), EnableIf::IsVoid<tReturn>>
      {
      };

      // TODO: Add ref qualified member functions.

      /////////////////////////////////////////////
      // Member Functions
      // Returns Something
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
        template <tFunctionSignature tFunction>
        inline static Any Invoke(std::vector<Any>& aArguments)
        {
          auto self = aArguments.at(0).As<tObject*>();

          size_t i = 1;

          // We get a warning for functions that don't have arguments and thus don't use these.
          UnusedArguments(aArguments, i);

          tReturn capture = (self->*tFunction)(aArguments.at(i++).As<tArguments>()...);
          Any toReturn{ capture, TypeId<tReturn>(), false == std::is_reference_v<tReturn> };
          return toReturn;
        }
      };

      // Void Return
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsVoid<tReturn>>
      {
        template <tFunctionSignature tFunction>
        inline static Any Invoke(std::vector<Any>& aArguments)
        {
          auto self = aArguments.at(0).As<tObject*>();

          size_t i = 1;

          // We get a warning for functions that don't have arguments and thus don't use these.
          UnusedArguments(aArguments, i);

          (self->*tFunction)(aArguments.at(i++).As<tArguments>()...);
          return Any{};
        }
      };

      /////////////////////////////////////////////
      // Noexcept Member Functions
      // Returns Something
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...) noexcept, EnableIf::IsNotVoid<tReturn>>
        : public FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
      };

      // Void Return
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...) noexcept, EnableIf::IsVoid<tReturn>>
        : public FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsVoid<tReturn>>
      {
      };

      /////////////////////////////////////////////
      // Const Member Functions
      // Returns Something
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...) const, EnableIf::IsNotVoid<tReturn>>
        : public FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
      };

      // Void Return
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...) const, EnableIf::IsVoid<tReturn>>
        : public FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsVoid<tReturn>>
      {
      };


      /////////////////////////////////////////////
      // Const Noexcept Member Functions
      // Returns Something
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...) const noexcept, EnableIf::IsNotVoid<tReturn>>
        : public FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsNotVoid<tReturn>>
      {
      };

      // Void Return
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionInvoker<tReturn(tObject::*)(tArguments...) const noexcept, EnableIf::IsVoid<tReturn>>
        : public FunctionInvoker<tReturn(tObject::*)(tArguments...), EnableIf::IsVoid<tReturn>>
      {
      };

      template <typename tReturn>
      struct FunctionMaker {};

      // Free/Static Member Function Pointer
      template <typename tReturn, typename... tArguments>
      struct FunctionMaker<tReturn(*)(tArguments...)>
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
          return FunctionInvoker<tFunctionSignature>::template Invoke<tFunction>(aArguments);
        }
      };

      // Free/Static Member Function
      template <typename tReturn, typename... tArguments>
      struct FunctionMaker<tReturn(tArguments...) >
        : public FunctionMaker<tReturn(*)(tArguments...)>
      {
      };
      
      // Noexcept Free/Static Member Function
      template <typename tReturn, typename... tArguments>
      struct FunctionMaker<tReturn(tArguments...) noexcept>
        : public FunctionMaker<tReturn(*)(tArguments...)>
      {
      };
      
      // Free/Static Member Function Pointer
      template <typename tReturn, typename... tArguments>
      struct FunctionMaker<tReturn(*)(tArguments...) noexcept>
        : public FunctionMaker<tReturn(*)(tArguments...)>
      {
      };
      

      // Member Function
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionMaker<tReturn(tObject::*)(tArguments...)>
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
          return FunctionInvoker<tFunctionSignature>::template Invoke<tFunction>(aArguments);
        }
      };

      // Noexcept Member Function
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionMaker<tReturn(tObject::*)(tArguments...) noexcept> :
        public FunctionMaker<tReturn(tObject::*)(tArguments...)>
      {
      };

      // Const Member Function
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionMaker<tReturn(tObject::*)(tArguments...) const> :
        public FunctionMaker<tReturn(tObject::*)(tArguments...)>
      {
      };

      // Const Noexcept Member Function
      template <typename tReturn, typename tObject, typename... tArguments>
      struct FunctionMaker<tReturn(tObject::*)(tArguments...) const noexcept> :
        public FunctionMaker<tReturn(tObject::*)(tArguments...)>
      {
      };

      template <tFunctionSignature tFunction>
      static Any Caller(std::vector<Any>& aArguments)
      {
        return FunctionMaker<tFunctionSignature>::template Invoke<tFunction>(aArguments);
      }

      static std::unique_ptr<Function> BindPassedFunction(const char *aName,
                                                          CallingType aCaller)
      {
        auto function = FunctionMaker<tFunctionSignature>::MakeFunction(aName);

        function->SetCaller(aCaller);

        return std::move(function);
      }

      template <tFunctionSignature tFunction>
      static std::unique_ptr<Function> BindFunction(const char *aName)
      {
        auto function = FunctionMaker<tFunctionSignature>::MakeFunction(aName);

        function->SetCaller(Caller<tFunction>);

        return std::move(function);
      }
    };

    template <>
    struct FunctionBinding<std::nullptr_t>
    {
      template <std::nullptr_t BoundFunc>
      static std::unique_ptr<Function> BindFunction(const char *aName)
      {
        UnusedArguments(aName);

        return std::unique_ptr<Function>();
      }
    };
  }
}