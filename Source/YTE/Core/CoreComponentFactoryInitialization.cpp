/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-12-14
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/CoreComponentFactoryInitilization.hpp"

#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/TestComponent.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/FacialAnimator.hpp"
#include "YTE/Graphics/FlybyCamera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/ImguiLayer.hpp"
#include "YTE/Graphics/Light.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Graphics/Skybox.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/SpriteText.hpp"
#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Graphics/TempDiffuseColoringComponent.hpp"
#include "YTE/Graphics/InfluenceMap.hpp"

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
#include "YTE/WWise/WWiseView.hpp"

#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"

namespace YTE
{
  using namespace std;

  void CoreComponentFactoryInitilization(Engine *aEngine, FactoryMap &aComponentFactories)
  {
    YTE::ComponentFactoryHelper helper{aEngine, &aComponentFactories };

    helper.CreateComponentFactory<WWiseEmitter>();
    helper.CreateComponentFactory<WWiseListener>();
    helper.CreateComponentFactory<WWiseView>();

    helper.CreateComponentFactory<ActionManager>();
    helper.CreateComponentFactory<TestComponent>();

    helper.CreateComponentFactory<Camera>();
    helper.CreateComponentFactory<FacialAnimator>();
    helper.CreateComponentFactory<FlybyCamera>();
    helper.CreateComponentFactory<GraphicsView>();
    helper.CreateComponentFactory<ImguiLayer>();
    helper.CreateComponentFactory<Light>();
    helper.CreateComponentFactory<Material>();
    helper.CreateComponentFactory<Model>();
    helper.CreateComponentFactory<ParticleEmitter>();
    helper.CreateComponentFactory<Skybox>();
    helper.CreateComponentFactory<Animator>();
    helper.CreateComponentFactory<FFT_WaterSimulation>();
    helper.CreateComponentFactory<TempDiffuseColoringComponent>();
    helper.CreateComponentFactory<InfluenceMap>();

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
