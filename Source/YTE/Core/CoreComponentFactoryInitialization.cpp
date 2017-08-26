/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-12-14
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/CoreComponentFactoryInitilization.hpp"

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/ComponentSystem.h"
#include "YTE/Core/TestComponent.hpp"

#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/CollisionBody.h"
#include "YTE/Physics/BoxCollider.h"
#include "YTE/Physics/CapsuleCollider.h"
#include "YTE/Physics/Collider.h"
#include "YTE/Physics/CollisionBody.h"
#include "YTE/Physics/CylinderCollider.h"
#include "YTE/Physics/GhostBody.h"
#include "YTE/Physics/MeshCollider.h"
#include "YTE/Physics/Orientation.h"
#include "YTE/Physics/PhysicsSystem.h"
#include "YTE/Physics/Reactive.h"
#include "YTE/Physics/RigidBody.h"
#include "YTE/Physics/SphereCollider.h"
#include "YTE/Physics/Transform.h"

#include "YTE/Utilities/String/String.h"

#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseListener.hpp"

namespace YTE
{

  #define DeclareComponent(component)                                                          \
  do                                                                                           \
  {                                                                                            \
                                                                                               \
    auto it = currComponentFactories.Find(##component::GetStaticType());                       \
                                                                                               \
    if (it == currComponentFactories.end())                                                    \
    {                                                                                          \
      currComponentFactories.Emplace(##component::GetStaticType(),                             \
	                                   std::make_unique<ComponentFactory<##component>>(aEngine));\
    }                                                                                          \
  } while(false)

  using namespace std;

  void CoreComponentFactoryInitilization(Engine *aEngine, FactoryMap &currComponentFactories)
  {
    FactoryMap::iterator it;

    DeclareComponent(WWiseEmitter);
    DeclareComponent(WWiseListener);

    DeclareComponent(TestComponent);

    DeclareComponent(Camera);
    DeclareComponent(GraphicsView);
    DeclareComponent(Model);

    //DeclareComponent(Collider)
    DeclareComponent(BoxCollider);
    DeclareComponent(CapsuleCollider);
    DeclareComponent(CollisionBody);
    DeclareComponent(CylinderCollider);
    DeclareComponent(GhostBody);
    DeclareComponent(MenuCollider);
    DeclareComponent(MeshCollider);
    DeclareComponent(Orientation);
    DeclareComponent(PhysicsSystem);
    DeclareComponent(Reactive);
    DeclareComponent(RigidBody);
    DeclareComponent(SphereCollider);
    DeclareComponent(Transform);
  }

  #undef DeclareComponent
}
