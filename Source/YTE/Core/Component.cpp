#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"


#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/ComponentSystem.h"


namespace YTE
{
  YTEDefineType(Component)
  {
    YTERegisterType(Component);

    YTEBindFunction(&Component::Remove, YTENoOverload, "Remove", YTENoNames).Description()
      = "Removes the component from its owner. This is delayed until the next frame.";
  
    YTEBindProperty(&Component::GetOwner, YTENoSetter, "Owner");
  }

  Component::Component(Composition *aOwner, Space *aSpace)
    : mOwner(aOwner), mSpace(aSpace), mGUID()
  {
    Engine *engine = mOwner->GetEngine();

    Component *collision = engine->StoreComponentGUID(this);

    while (collision)
    {
      if (collision == this)
      {
        break;
      }
      else
      {
        mGUID = GlobalUniqueIdentifier();
        collision = engine->StoreComponentGUID(this);
      }
    }
  }

  Component::~Component()
  {
    mOwner->GetEngine()->RemoveComponentGUID(mGUID);
  }


  RSValue Component::Serialize(RSAllocator &aAllocator)
  {
    return SerializeByType<Component*>(aAllocator, this, GetType());
  }

  RSValue Component::RemoveSerialized()
  {
    RSAllocator allocator;
    auto data = Serialize(allocator);
    Remove();
    return data;
  }


  void Component::DebugBreak()
  {
    debugbreak();
  }

  GlobalUniqueIdentifier& Component::GetGUID()
  {
    return mGUID;
  }

  bool Component::SetGUID(GlobalUniqueIdentifier aGUID)
  {
    YTE::Engine *engine = mOwner->GetEngine();

    bool collision = engine->CheckForComponentGUIDCollision(aGUID);

    if (collision)
    {
      engine->RemoveComponentGUID(mGUID);
    }

    mGUID = aGUID;

    engine->StoreComponentGUID(this);

    return collision;
  }

  void Component::Remove()
  {
    mOwner->RemoveComponent(this);
  }

  YTEDefineType(ComponentDependencies)
  {
    YTERegisterType(ComponentDependencies);
  }

  ComponentDependencies::ComponentDependencies(DocumentedObject *aObject,
                        std::vector<std::vector<Type*>> aTypes)
  {
    auto typeAddingTo = dynamic_cast<Type*>(aObject);

    YTEUnusedArgument(typeAddingTo);
    DebugObjection(nullptr == typeAddingTo,
                   "ComponentDependencies Attribute being added to unknown object type.");

    for (auto typeOptions : aTypes)
    {
      std::vector<Type*> types;
      for (auto type : typeOptions)
      {
        DebugObjection(false == type->IsA(Component::GetStaticType()),
                       "Type %s is not a component but it's being used as a component"
                       " dependency on type %s",
                       type->GetName().c_str(),
                       typeAddingTo->GetName().c_str());

        types.emplace_back(type);
      }

      mTypes.emplace_back(std::move(types));
    }
  }
}
