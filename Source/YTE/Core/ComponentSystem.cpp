/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-12-14
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/ComponentSystem.h"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/CoreComponentFactoryInitilization.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

namespace YTE
{
  DefineType(ComponentSystem)
  {
    YTERegisterType(ComponentSystem);

  }


  ComponentSystem::ComponentSystem(Composition *aOwner, 
                                   RSValue *aProperties)
    : Component(aOwner, nullptr)
  {
    CoreComponentFactoryInitilization(static_cast<Engine*>(mOwner), mComponentFactories);
    GetOwner()->CONNECT(Events::BoundTypeChanged, this, &ComponentSystem::BoundTypeChangedHandler);
  };

  void ComponentSystem::BoundTypeChangedHandler(BoundTypeChanged *aEvent)
  {
    auto iterator = mComponentFactories.Find(aEvent->aOldType);

    if (iterator != mComponentFactories.end())
    {
      mComponentFactories.ChangeKey(iterator, aEvent->aNewType);
    }

    auto engine = static_cast<Engine*>(GetOwner());
  }

  void ComponentSystem::FactorySetup(FactorySetupCallback aFunctionPtr)
  {
    if (aFunctionPtr != nullptr)
    {
      aFunctionPtr(mComponentFactories);
    }
  }
    
  std::pair<StringComponentFactory *, UniquePointer<Component>>
    ComponentSystem::MakeComponent(BoundType *aType,
                                    Composition *aOwner, 
                                    RSValue *aProperties)
  {
    auto it = mComponentFactories.Find(aType);

    if (it == mComponentFactories.end())
    {
      std::cout << "No factory of this type:" << aType->GetName().c_str() << std::endl;
    }

    auto &factory = mComponentFactories.Find(aType)->second;
    return std::make_pair(factory.get(), factory->MakeComponent(aOwner, mSpace, aProperties));
  }
} // End yte namespace.

