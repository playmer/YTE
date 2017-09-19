#pragma once

#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Meta/Function.hpp"

namespace YTE
{
  // TODO: Have some requirements on the types of setters and getters.
  class Property : public DocumentedObject
  {
  public:
    YTEDeclareType(Property)

    Property(Property&) = delete;

    Property(const char *aName,
             std::unique_ptr<Function> aGetter,
             std::unique_ptr<Function> aSetter)
      : mName(aName)
      , mGetter(std::move(aGetter))
      , mSetter(std::move(aSetter))
    {
      runtime_assert((nullptr != mGetter) || (nullptr != mSetter),
                     "At least one of the getter and setter must be set.");
    }

    const std::string& GetName() const { return mName; }
    Function* GetGetter() { return mGetter.get(); }
    Function* GetSetter() { return mSetter.get(); }
  protected:
    std::string mName;
    std::unique_ptr<Function> mGetter;
    std::unique_ptr<Function> mSetter;
  };



  template <typename GetterFunctionSignature, GetterFunctionSignature GetterFunction,
            typename SetterFunctionSignature, SetterFunctionSignature SetterFunction>
  static Property& BindProperty(const char *aName, Type *aType)
  {
    auto getter = Binding<GetterFunctionSignature>:: template BindFunction<GetterFunction>("Getter");
    auto setter = Binding<SetterFunctionSignature>:: template BindFunction<SetterFunction>("Setter");

    auto property = std::make_unique<Property>(aName, std::move(getter), std::move(setter));
    auto ptr = property.get();
    aType->AddProperty(std::move(property));

    return *ptr;
  }
}