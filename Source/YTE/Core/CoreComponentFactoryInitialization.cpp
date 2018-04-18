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

#include "YTE/GameComponents/AudioTest.hpp"
#include "YTE/GameComponents/BoatController.hpp"
#include "YTE/GameComponents/BoatParticles.hpp"
#include "YTE/GameComponents/BoatPhysics.hpp"
#include "YTE/GameComponents/Dialogue.hpp"
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/GameComponents/DialogueElement.hpp"
#include "YTE/GameComponents/DialogueElementDefault.hpp"
#include "YTE/GameComponents/HudController.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/AddComponentToEngine.hpp"
#include "YTE/GameComponents/CameraAnchor.hpp"
#include "YTE/GameComponents/CameraBoom.hpp"
#include "YTE/GameComponents/CameraController.hpp"
#include "YTE/GameComponents/Menu/MenuController.hpp"
#include "YTE/GameComponents/Menu/RootMenu.hpp"
#include "YTE/GameComponents/Menu/LaunchMenu.hpp"
#include "YTE/GameComponents/Menu/ExitMenu.hpp"
#include "YTE/GameComponents/Menu/LaunchLevel.hpp"
#include "YTE/GameComponents/Menu/LaunchGame.hpp"
#include "YTE/GameComponents/Menu/QuitGame.hpp"
#include "YTE/GameComponents/Menu/Button.hpp"
#include "YTE/GameComponents/Menu/ButtonAnimate.hpp"
#include "YTE/GameComponents/Credits.hpp"
#include "YTE/GameComponents/SampleAction.hpp"
#include "YTE/GameComponents/demo_InsideZone.hpp"
#include "YTE/GameComponents/Zone.hpp"
#include "YTE/GameComponents/Island.hpp"
#include "YTE/GameComponents/TestingComponent.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"
#include "YTE/GameComponents/DaisyDialogue.hpp"
#include "YTE/GameComponents/BasilDialogue.hpp"
#include "YTE/GameComponents/QuestLogic.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"
#include "YTE/GameComponents/QuestProgressionTrigger.hpp"
#include "YTE/GameComponents/JohnMovement.hpp"
#include "YTE/GameComponents/DaisyMovement.hpp"
#include "YTE/GameComponents/BasilMovement.hpp"
#include "YTE/GameComponents/StarMovement.hpp"
#include "YTE/GameComponents/JohnTutorial.hpp"
#include "YTE/GameComponents/DaisyTutorial.hpp"
#include "YTE/GameComponents/BasilTutorial.hpp"
#include "YTE/GameComponents/OceanCreatureBehavior.hpp"
#include "YTE/GameComponents/ProgressionParticles.hpp"

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

    helper.CreateComponentFactory<AudioTest>();
    helper.CreateComponentFactory<BoatController>();
    helper.CreateComponentFactory<BoatParticles>();
    helper.CreateComponentFactory<BoatPhysics>();
    helper.CreateComponentFactory<InputInterpreter>();
    helper.CreateComponentFactory<AddComponentToEngine>();
    helper.CreateComponentFactory<CameraAnchor>();
    helper.CreateComponentFactory<CameraBoom>();
    helper.CreateComponentFactory<CameraController>();

    helper.CreateComponentFactory<Credits>();
    helper.CreateComponentFactory<MenuController>();
    helper.CreateComponentFactory<RootMenu>();
    helper.CreateComponentFactory<LaunchMenu>();
    helper.CreateComponentFactory<ExitMenu>();
    helper.CreateComponentFactory<LaunchLevel>();
    helper.CreateComponentFactory<LaunchGame>();
    helper.CreateComponentFactory<QuitGame>();
    helper.CreateComponentFactory<Button>();
    helper.CreateComponentFactory<ButtonAnimate>();

    helper.CreateComponentFactory<SampleAction>();
    helper.CreateComponentFactory<demo_InsideZone>();
    helper.CreateComponentFactory<Dialogue>();
    helper.CreateComponentFactory<DialogueDirector>();
    helper.CreateComponentFactory<DialogueElement>();
    helper.CreateComponentFactory<DialogueElementDefault>();
    helper.CreateComponentFactory<HudController>();
    helper.CreateComponentFactory<Zone>();
    helper.CreateComponentFactory<Island>();
    helper.CreateComponentFactory<TestingComponent>();
    helper.CreateComponentFactory<JohnDialogue>();
    helper.CreateComponentFactory<DaisyDialogue>();
    helper.CreateComponentFactory<BasilDialogue>();
    helper.CreateComponentFactory<QuestLogic>();
    helper.CreateComponentFactory<NoticeBoard>();
    helper.CreateComponentFactory<QuestProgressionTrigger>();
    helper.CreateComponentFactory<JohnMovement>();
    helper.CreateComponentFactory<DaisyMovement>();
    helper.CreateComponentFactory<BasilMovement>();
    helper.CreateComponentFactory<StarMovement>();
    helper.CreateComponentFactory<JohnTutorial>();
    helper.CreateComponentFactory<DaisyTutorial>();
    helper.CreateComponentFactory<BasilTutorial>();
    helper.CreateComponentFactory<OceanCreatureBehavior>();
    helper.CreateComponentFactory<ProgressionParticles>();

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
