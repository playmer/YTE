#include "YTE/CSharp/CSharpComponent.hpp"
#include "YTE/CSharp/CSharpComponentFactory.hpp"

namespace YTE
{
  CSharpComponentFactory::CSharpComponentFactory(Engine *aEngine,
                                                 MonoDomain *aDomain,
                                                 MonoClass *aClass)
    : StringComponentFactory(aEngine)
    , mInitializeFunction(nullptr)
    , mClass(aClass)
    , mDomain(aDomain)
  {
    auto method = mono_class_get_method_from_name(aClass, "Initialize", 0);

    if (nullptr != method)
    {
      auto fn = mono_method_get_unmanaged_thunk(method);

      mInitializeFunction = static_cast<CSharpComponentInitFn>(fn);
    }
  }

  
  UniquePointer<Component> CSharpComponentFactory::MakeComponent(Composition *aOwner,
                                                                 Space *aSpace, 
                                                                 RSValue *aProperties)
  {
    return std::make_unique<CSharpComponent>(mInitializeFunction, 
                                             mDomain, 
                                             mClass, 
                                             aOwner, 
                                             aSpace, 
                                             aProperties);
  }
}