#include "YTE/Meta/Meta.hpp"
#include "YTE/Meta/Function.hpp"

namespace YTE
{
  YTEDefineType(Function)
  {
    RegisterType<Function>();

    YTEBindProperty(&Function::GetOwningType, YTENoSetter, "OwningType")
      .Description() = "Type that owns this Function.";
    YTEBindProperty(&Function::GetName, YTENoSetter, "Name")
      .Description() = "Name of the property.";
    YTEBindProperty(&Function::GetReturnType, YTENoSetter, "ReturnType")
      .Description() = "The return type of the function.";
    YTEBindProperty(&Function::IsStaticOrFree, YTENoSetter, "StaticOrFree")
      .Description() = "Lets you know if this function is a static or free function, as in not a member function.";
  }

  Function::Parameter::Parameter(Type *aType, const char *aName)
    : mType(aType)
    , mName(aName)
  {

  }

  Type *mType;
  std::string mName;

  Function::Function(const char *aName, 
                     Type *aReturnType, 
                     Type *aOwningType)
    : mName(aName)
    , mReturnType(aReturnType)
    , mCaller(nullptr)
    , mOwningType(aOwningType)
    , mStaticOrFree(nullptr == aOwningType)
  {
    if ((aOwningType == nullptr) && (mStaticOrFree == false))
    {
      runtime_assert(false,
        "A function without an owning type is, by definition, static.");
    }
  }

  Any Function::Invoke(std::vector<Any> &aArguments) const
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

  void Function::AddParameter(Type *aType, const char *aName)
  {
    mParameters.emplace_back(aType, aName);
  }

  void Function::SetCaller(CallingFunction aCaller)
  {
    mCaller = aCaller;
  }

  const std::string& Function::GetName() const
  {
    return mName;
  }

  void Function::SetOwningType(Type *aOwningType)
  {
    mOwningType = aOwningType;
  }

  Type* Function::GetOwningType() const
  {
    return mOwningType;
  }

  bool Function::IsSame(Function &aFunction) const
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


  Type* Function::GetReturnType() const
  {
    return mReturnType;
  }

  bool Function::IsStaticOrFree()
  {
    return mStaticOrFree;
  }

  void Function::SetParameterNames(std::initializer_list<const char *> aNames)
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

  std::vector<Function::Parameter>& Function::GetParameters()
  {
    return mParameters;
  }
}
