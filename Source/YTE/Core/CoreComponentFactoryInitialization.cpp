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
#include "YTE/Graphics/Skybox.hpp"

#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/Physics/CapsuleCollider.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/CylinderCollider.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/MeshCollider.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/Reactive.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/SphereCollider.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/String/String.h"

#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseListener.hpp"

#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"

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
    DeclareComponent(WWiseEmitter);
    DeclareComponent(WWiseListener);

    DeclareComponent(TestComponent);

    DeclareComponent(Camera);
    DeclareComponent(GraphicsView);
    DeclareComponent(Model);
    DeclareComponent(Skybox);
    DeclareComponent(Animator);

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
