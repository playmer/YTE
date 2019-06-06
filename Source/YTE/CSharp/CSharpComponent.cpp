#include "YTE/CSharp/CSharpComponent.hpp"

namespace YTE
{
  CSharpComponent::CSharpComponent(CSharpComponentInitFn aInitFn,
                                   MonoDomain *aDomain,
                                   MonoClass *aClass,
                                   Composition *aOwner,
                                   Space *aSpace,
                                   RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDomain(aDomain)
    , mClass(aClass)
    , mObject(nullptr)
    , mInitFn(aInitFn)
  {
    YTEUnusedArgument(aProperties);

    mObject = mono_object_new(aDomain, aClass);
    mono_runtime_object_init(mObject);
    mGCHandle = mono_gchandle_new(mObject, true);
  }

  void CSharpComponent::Initialize()
  {
    if (mInitFn)
    {
      MonoException *except;
      mInitFn(mObject, &except);
    }
  }

  CSharpComponent::~CSharpComponent()
  {
    mono_gchandle_free(mGCHandle);
  }
}