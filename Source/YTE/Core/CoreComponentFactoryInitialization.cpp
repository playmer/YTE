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
#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/TestComponent.hpp"

#include "YTE/GameComponents/AudioTest.hpp"
#include "YTE/GameComponents/BoatController.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/AddComponentToEngine.hpp"
#include "YTE/GameComponents/CameraController.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Light.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Graphics/Skybox.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/SpriteText.hpp"
#include "YTE/Graphics/TempDiffuseColoringComponent.hpp"

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

#include "YTE/Utilities/String/String.hpp"

#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseListener.hpp"

#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"

namespace YTE
{
  using namespace std;

  void CoreComponentFactoryInitilization(Engine *aEngine, FactoryMap &currComponentFactories)
  {
    YTE::ComponentFactoryHelper helper{aEngine, &currComponentFactories};

    helper.CreateComponentFactory<WWiseEmitter>();
    helper.CreateComponentFactory<WWiseListener>();

    helper.CreateComponentFactory<TestComponent>();

    helper.CreateComponentFactory<AudioTest>();
    helper.CreateComponentFactory<BoatController>();
    helper.CreateComponentFactory<InputInterpreter>();
    helper.CreateComponentFactory<AddComponentToEngine>();
    helper.CreateComponentFactory<CameraController>();

    helper.CreateComponentFactory<Camera>();
    helper.CreateComponentFactory<GraphicsView>();
    helper.CreateComponentFactory<Light>();
    helper.CreateComponentFactory<Material>();
    helper.CreateComponentFactory<Model>();
    helper.CreateComponentFactory<ParticleEmitter>();
    helper.CreateComponentFactory<Skybox>();
    helper.CreateComponentFactory<Animator>();
    helper.CreateComponentFactory<TempDiffuseColoringComponent>();

    //helper.CreateComponentFactory<Collider>();
    helper.CreateComponentFactory<BoxCollider>();
    helper.CreateComponentFactory<CapsuleCollider>();
    helper.CreateComponentFactory<CollisionBody>();
    helper.CreateComponentFactory<CylinderCollider>();
    helper.CreateComponentFactory<GhostBody>();
    helper.CreateComponentFactory<MenuCollider>();
    helper.CreateComponentFactory<MeshCollider>();
    helper.CreateComponentFactory<Orientation>();
    helper.CreateComponentFactory<PhysicsSystem>();
    helper.CreateComponentFactory<Reactive>();
    helper.CreateComponentFactory<RigidBody>();
    helper.CreateComponentFactory<SphereCollider>();
    helper.CreateComponentFactory<Sprite>();
    helper.CreateComponentFactory<SpriteText>();
    helper.CreateComponentFactory<Transform>();
  }
}
