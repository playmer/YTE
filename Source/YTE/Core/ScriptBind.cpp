/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-12-14
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/ScriptBind.hpp"

#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Object.hpp"
#include "YTE/Core/TestComponent.hpp"
#include "YTE/Core/Threading/JobSystem.hpp"

#include "YTE/Core/EventHandler.hpp"

#include "YTE/GameComponents/AudioTest.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Light.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Graphics/Skybox.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/TempDiffuseColoringComponent.hpp"

#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/Physics/Body.hpp"
#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/DebugDraw.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/CapsuleCollider.hpp"
#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/CylinderCollider.hpp"
#include "YTE/Physics/MeshCollider.hpp"
#include "YTE/Physics/MenuCollider.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/Reactive.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/SphereCollider.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/GamepadSystem.hpp"
#include "YTE/Platform/Window.hpp"

#include "YTE/WWise/WWiseSystem.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseListener.hpp"

namespace YTE
{
  void InitializeYTETypes()
  {
    InitializeType<Component>();
    InitializeType<ComponentSystem>();
    InitializeType<Composition>();
    InitializeType<Engine>();
    InitializeType<JobSystem>();
    InitializeType<Object>();
    InitializeType<Space>();
    InitializeType<TestComponent>();

    InitializeType<AudioTest>();

    InitializeType<Event>();
    InitializeType<EventHandler>();
    InitializeType<LogicUpdate>();
    InitializeType<MouseButtonEvent>();
    InitializeType<MouseMoveEvent>();
    InitializeType<MouseWheelEvent>();
    InitializeType<KeyboardEvent>();
    InitializeType<XboxButtonEvent>();
    InitializeType<XboxFlickEvent>();
    InitializeType<TransformChanged>();
    InitializeType<OrientationChanged>();
    InitializeType<CollisionEvent>();
    InitializeType<CollisionStarted>();
    InitializeType<CollisionPersisted>();
    InitializeType<CollisionEnded>();
    InitializeType<MouseEnter>();
    InitializeType<MouseExit>();

    InitializeType<Animator>();
    InitializeType<Animation>();
    InitializeType<Camera>();
    InitializeType<GraphicsSystem>();
    InitializeType<GraphicsView>();
    InitializeType<Material>();
    InitializeType<MaterialRepresentation>();
    InitializeType<Model>();
    InitializeType<ParticleEmitter>();
    InitializeType<Light>();
    InitializeType<ModelChanged>();
    InitializeType<Skybox>();
    InitializeType<Sprite>();
    InitializeType<TempDiffuseColoringComponent>();

    InitializeType<Body>();
    InitializeType<BoxCollider>();
    InitializeType<CapsuleCollider>();
    InitializeType<CollisionBody>();
    InitializeType<Collider>();
    InitializeType<GhostBody>();
    InitializeType<CylinderCollider>();
    InitializeType<MeshCollider>();
    InitializeType<MenuCollider>();
    InitializeType<Orientation>();
    InitializeType<PhysicsSystem>();
    InitializeType<RayCollisionInfo>();
    InitializeType<RigidBody>();
    InitializeType<Reactive>();
    InitializeType<SphereCollider>();
    InitializeType<Transform>();

    InitializeType<Keyboard>();
    InitializeType<Mouse>();
    InitializeType<GamepadSystem>();
    InitializeType<XboxController>();
    InitializeType<Window>();
    
    InitializeType<WWiseSystem>();
    InitializeType<WWiseEmitter>();
    InitializeType<WWiseListener>();
    
    InitializeType<void>();
    InitializeType<bool>();
    InitializeType<s8>();
    InitializeType<i8>();
    InitializeType<i16>();
    InitializeType<i32>();
    InitializeType<i64>();
    InitializeType<u8>();
    InitializeType<u16>();
    InitializeType<u32>();
    InitializeType<u64>();
    InitializeType<float>();
    InitializeType<double>();
    InitializeType<std::string>();
    InitializeType<YTE::String>();
    InitializeType<glm::vec2>();
    InitializeType<glm::vec3>();
    InitializeType<glm::vec4>();
    InitializeType<glm::quat>();
    InitializeType<btIDebugDraw::DebugDrawModes>();
    InitializeType<YTE::Controller_Id>();
    InitializeType<YTE::Xbox_Buttons>();
    InitializeType<YTE::Mouse_Buttons>();
    InitializeType<YTE::Keys>();
  }
}
