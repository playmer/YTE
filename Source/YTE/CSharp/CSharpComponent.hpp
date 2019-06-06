#pragma once

#include <mono/jit/jit.h> 

#include "YTE/Core/Component.hpp"

#include "YTE/CSharp/ForwardDeclarations.hpp"

namespace YTE
{
  class CSharpComponent : public Component
  {
  public:
    CSharpComponent(CSharpComponentInitFn aInitFn,
                    MonoDomain *aDomain,
                    MonoClass *aClass,
                    Composition *aOwner, 
                    Space *aSpace, 
                    RSValue *aProperties);

    ~CSharpComponent() override;

    void Initialize() override;

  private:
    MonoDomain *mDomain;
    MonoClass *mClass;
    MonoObject *mObject;
    CSharpComponentInitFn mInitFn;
    u32 mGCHandle;
  };
}