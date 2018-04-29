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
             std::unique_ptr<Function> aSetter);

    Type* GetOwningType() { return mOwningType; }
    Type* GetPropertyType() { return mType; }
    const std::string& GetName() const { return mName; }
    Function* GetGetter() { return mGetter.get(); }
    Function* GetSetter() { return mSetter.get(); }
    void SetPropertyType(Type *aType) { mType = aType; }

  protected:
    Type *mOwningType;
    Type *mType;
    std::string mName;
    std::unique_ptr<Function> mGetter;
    std::unique_ptr<Function> mSetter;
  };

  


  template <typename GetterFunctionSignature, GetterFunctionSignature GetterFunction,
            typename SetterFunctionSignature, SetterFunctionSignature SetterFunction>
  static Property& BindProperty(const char *aName, Type *aType)
  {
    auto getter = Detail::Meta::FunctionBinding<GetterFunctionSignature>:: template BindFunction<GetterFunction>("Getter");
    auto setter = Detail::Meta::FunctionBinding<SetterFunctionSignature>:: template BindFunction<SetterFunction>("Setter");

    auto property = std::make_unique<Property>(aName, std::move(getter), std::move(setter));
    
    auto ptr = aType->AddProperty(std::move(property));

    return *ptr;
  }
}