#pragma once

#include "YTE/Core/ComponentFactory.hpp"

#include "YTE/CSharp/CSharpComponent.hpp"

namespace YTE
{
  class CSharpComponentFactory : public StringComponentFactory
  {
  public:
    CSharpComponentFactory(Engine *aEngine, 
                           MonoDomain *aDomain,
                           MonoClass *aClass);

    UniquePointer<Component> MakeComponent(Composition *aOwner, 
                                           Space *aSpace, 
                                           RSValue *aProperties) override;

    CSharpComponentFactory(Engine *aEngine) : StringComponentFactory(aEngine) {};
    virtual ~CSharpComponentFactory() { };

  private:
    CSharpComponentInitFn mInitializeFunction;
    MonoClass *mClass;
    MonoDomain *mDomain;
  };
}