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
      Parameter(Type *aType, const char *aName)
        : mType(aType)
        , mName(aName)
      {

      }

      Type *mType;
      std::string mName;
    };

    Function(const char *aName, Type *aReturnType, Type *aOwningType, bool aStaticOrFree)
      : mName(aName)
      , mReturnType(aReturnType)
      , mCaller(nullptr)
      , mOwningType(aOwningType)
      , mStaticOrFree(aStaticOrFree)
    {
      if ((aOwningType == nullptr) && (mStaticOrFree == false))
      {
        runtime_assert(false,
                       "A function without an owning type is, by definition, static.");
      }
    }

    Any Invoke(std::vector<Any> &aArguments) const
    {
      if (mParameters.size() != aArguments.size())
      {
        std::cout << "Different argument amounts." << std::endl;
        return Any();
      }

      for (size_t i = 0; i < mParameters.size(); ++i)
      {
        auto parameterType = mParameters[i].mType->GetMostBasicType();
        auto argumentType = aArguments[i].mType->GetMostBasicType();

        if (false == argumentType->IsA(parameterType) &&
            false == parameterType->IsA(argumentType))
        {
          std::cout << "Different argument types." << std::endl;
          return Any();
        }
      }

      return mCaller(aArguments);
    }

    // Will return default constructed Any if the arguments fail.
    template <typename ...Arguments>
    Any Invoke(Arguments...aArguments) const
    {
      auto args = Any::FromVariadic<Arguments...>(aArguments...);

      return Invoke(args);
    }

    void AddParameter(Type *aType, const char *aName = "")
    {
      mParameters.emplace_back(aType, aName);
    }

    void SetCaller(CallingFunction aCaller)
    {
      mCaller = aCaller;
    }

    const std::string& GetName() const
    {
      return mName;
    }

    void SetOwningType(Type *aOwningType) 
    {
      mOwningType = aOwningType;
    }

    Type* GetOwningType() const
    {
      return mOwningType;
    }

    bool IsSame(Function &aFunction) const
    {
      if (mName != aFunction.mName)
      {
        return false;
      }

      if (mReturnType != aFunction.mReturnType)
      {
        return false;
      }

      if (mParameters.size() == aFunction.mParameters.size())
      {
        for (size_t i = 0; i < mParameters.size(); ++i)
        {
          if (mParameters[i].mType != aFunction.mParameters[i].mType)
          {
            return false;
          }
        }
      }

      return true;
    }


    Type* GetReturnType() const
    {
      return mReturnType;
    }

    bool IsStaticOrFree()
    {
      return mStaticOrFree;
    }

    void SetParameterNames(std::initializer_list<const char *> aNames)
    {
      // Member Function
      if (false == mStaticOrFree && 
          aNames.size() == (mParameters.size() - 1))
      {
        mParameters[0].mName = "aThisPointer";

        for (size_t i = 1; i < mParameters.size(); ++i)
        {
          mParameters[i].mName = *(aNames.begin() + i - 1);
        }
      }
      else if (aNames.size() == mParameters.size())
      {
        for (size_t i = 0; i < mParameters.size(); ++i)
        {
          mParameters[i].mName = *(aNames.begin() + i);
        }
      }
      else
      {
        for (size_t i = 0; i < mParameters.size(); ++i)
        {
          std::string argumentName{ "aArgument" };
          argumentName += std::to_string(i);
          mParameters[i].mName = argumentName;
        }
      }
    }

    std::vector<Parameter>& GetParameters()
    {
      return mParameters;
    }

  private:
    std::string mName;
    std::vector<Parameter> mParameters;
    Type *mReturnType;
    CallingFunction mCaller;
    Type *mOwningType;
    bool mStaticOrFree;
  };

  template <typename T>
  inline void AddParameter(Function *aFunction)
  {
    aFunction->AddParameter(TypeId<T>());
  }

  template <typename... Rest> struct ParseArguments;

  template <>
  struct ParseArguments<>
  {
    inline static void Parse(Function *)
    {
    }
  };

  template <typename First, typename... Rest>
  struct ParseArguments<First, Rest...>
  {
    inline static void Parse(Function *aFunction)
    {
      AddParameter<First>(aFunction);
      ParseArguments<Rest...>::Parse(aFunction);
    }
  };

  template <typename Return>
  struct Binding {};

  ///////////////////////////////////////////////////////////////////////////////
  // Free/Static Functions
  ///////////////////////////////////////////////////////////////////////////////
  template <typename Return, typename... Arguments>
  struct Binding<Return(*)(Arguments...)>
  {
    using FunctionSignature = Return(*)(Arguments...);
    using CallingType = Any(*)(std::vector<Any>&);

    template <FunctionSignature BoundFunc>
    static Any Caller(std::vector<Any>& aArguments)
    {
      size_t i = 0;

      // We get a warning for functions that don't have arguments and thus don't use these.
      YTEUnusedArgument(aArguments);
      YTEUnusedArgument(i);

      if constexpr(std::is_void<Return>::value)
      {
        BoundFunc(aArguments.at(i++).As<Arguments>()...);
        return Any{};
      }
      else
      {
        Return capture = BoundFunc(aArguments.at(i++).As<Arguments>()...);
        Any toReturn{ capture, TypeId<Return>(), false == std::is_reference<Return>::value };
        return toReturn;
      }
    }

    static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
    {
      auto function = std::make_unique<Function>(name, TypeId<Return>(), nullptr, true);
      ParseArguments<Arguments...>::Parse(function.get());

      function->SetCaller(aCaller);

      return std::move(function);
    }

    template <FunctionSignature BoundFunc>
    static std::unique_ptr<Function> BindFunction(const char *name)
    {
      auto function = std::make_unique<Function>(name, TypeId<Return>(), nullptr, true);
      ParseArguments<Arguments...>::Parse(function.get());

      function->SetCaller(Caller<BoundFunc>);

      return std::move(function);
    }
  };

  ///////////////////////////////////////////////////////////////////////////////
  // Member Functions
  ///////////////////////////////////////////////////////////////////////////////
  template <typename Return, typename ObjectType, typename... Arguments>
  struct Binding<Return(ObjectType::*)(Arguments...)>
  {
    using FunctionSignature = Return(ObjectType::*)(Arguments...);
    using CallingType = Any(*)(std::vector<Any>&);

    template <FunctionSignature BoundFunc>
    static Any Caller(std::vector<Any>& aArguments)
    {
      auto self = aArguments.at(0).As<ObjectType*>();

      size_t i = 1;

      // We get a warning for functions that don't have arguments and thus don't use these.
      YTEUnusedArgument(aArguments);
      YTEUnusedArgument(i);

      if constexpr(std::is_void<Return>::value)
      {
        (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
        return Any{};
      }
      else
      {
        Return capture = (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
        Any toReturn{ capture, TypeId<Return>(), false == std::is_reference<Return>::value };
        return toReturn;
      }
    }

    static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
    {
      auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
      function->AddParameter(TypeId<ObjectType*>());
      ParseArguments<Arguments...>::Parse(function.get());

      function->SetCaller(aCaller);

      return std::move(function);
    }

    template <FunctionSignature BoundFunc>
    static std::unique_ptr<Function> BindFunction(const char *name)
    {
      auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
      function->AddParameter(TypeId<ObjectType*>());
      ParseArguments<Arguments...>::Parse(function.get());

      function->SetCaller(Caller<BoundFunc>);

      return std::move(function);
    }
  };


  ///////////////////////////////////////////////////////////////////////////////
  // Const Member Functions
  ///////////////////////////////////////////////////////////////////////////////
  template <typename Return, typename ObjectType, typename... Arguments>
  struct Binding<Return(ObjectType::*)(Arguments...) const>
  {
    using FunctionSignature = Return(ObjectType::*)(Arguments...) const;
    using CallingType = Any(*)(std::vector<Any>&);

    template <FunctionSignature BoundFunc>
    static Any Caller(std::vector<Any>& aArguments)
    {
      auto self = aArguments.at(0).As<ObjectType*>();

      size_t i = 1;

      // We get a warning for functions that don't have arguments and thus don't use these.
      YTEUnusedArgument(aArguments);
      YTEUnusedArgument(i);


      if constexpr(std::is_void<Return>::value)
      {
        (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
        return Any{};
      }
      else
      {
        Return capture = (self->*BoundFunc)(aArguments.at(i++).As<Arguments>()...);
        Any toReturn{ capture, TypeId<Return>(), false == std::is_reference<Return>::value };
        return toReturn;
      }
    }

    static std::unique_ptr<Function> BindPassedFunction(const char *name, CallingType aCaller)
    {
      auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
      function->AddParameter(TypeId<ObjectType*>());
      ParseArguments<Arguments...>::Parse(function.get());

      function->SetCaller(aCaller);

      return std::move(function);
    }

    template <FunctionSignature BoundFunc>
    static std::unique_ptr<Function> BindFunction(const char *name)
    {
      auto function = std::make_unique<Function>(name, TypeId<Return>(), TypeId<ObjectType>(), false);
      function->AddParameter(TypeId<ObjectType*>());
      ParseArguments<Arguments...>::Parse(function.get());

      function->SetCaller(Caller<BoundFunc>);

      return std::move(function);
    }
  };

  template <>
  struct Binding<nullptr_t>
  {
    template <nullptr_t BoundFunc>
    static std::unique_ptr<Function> BindFunction(const char *name)
    {
      YTEUnusedArgument(name);

      return std::unique_ptr<Function>();
    }
  };

  template <typename FunctionSignature, FunctionSignature aBoundFunction, size_t aSize = 0>
  static Function& BindFunction(const char *name, Type *aType, std::initializer_list<const char *> aParameterNames)
  {
    static_assert(aSize == CountFunctionArguments<FunctionSignature>::Size() || aSize == 0,
                  "If passing names of function parameters you must pass either exactly as many names as there are arguments, or 0.");

    auto function = Binding<FunctionSignature>:: template BindFunction<aBoundFunction>(name);
    function->SetParameterNames(aParameterNames);
    function->SetOwningType(aType);
    
    auto ptr = aType->AddFunction(std::move(function));
    
    return *ptr;
  }
}