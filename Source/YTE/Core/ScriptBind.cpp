/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-12-14
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/ScriptBind.hpp"

#include "YTE/Core/ComponentSystem.h"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Object.hpp"
#include "YTE/Core/JobSystem.h"
#include "YTE/Core/TestComponent.hpp"

#include "YTE/Event/Events.h"
#include "YTE/Event/StandardEvents.h"

#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/BoxCollider.h"
#include "YTE/Physics/Body.h"
#include "YTE/Physics/CollisionBody.h"
#include "YTE/Physics/DebugDraw.h"
#include "YTE/Physics/GhostBody.h"
#include "YTE/Physics/CapsuleCollider.h"
#include "YTE/Physics/CollisionBody.h"
#include "YTE/Physics/Collider.h"
#include "YTE/Physics/CylinderCollider.h"
#include "YTE/Physics/MeshCollider.h"
#include "YTE/Physics/MenuCollider.h"
#include "YTE/Physics/Orientation.h"
#include "YTE/Physics/PhysicsSystem.h"
#include "YTE/Physics/Reactive.h"
#include "YTE/Physics/RigidBody.h"
#include "YTE/Physics/SphereCollider.h"
#include "YTE/Physics/Transform.h"

#include "YTE/Platform/Keyboard.h"
#include "YTE/Platform/Mouse.h"
#include "YTE/Platform/Gamepad.h"
#include "YTE/Platform/GamepadSystem.h"
#include "YTE/Platform/Window.h"

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

    InitializeType<Event>();
    InitializeType<BaseEventHandler>();
    InitializeType<LogicUpdate>();
    InitializeType<MouseButtonEvent>();
    InitializeType<MouseMoveEvent>();
    InitializeType<MouseWheelEvent>();
    InitializeType<KeyboardEvent>();
    InitializeType<XboxButtonEvent>();
    InitializeType<XboxFlickEvent>();
    InitializeType<ScaleChanged>();
    InitializeType<RotationChanged>();
    InitializeType<PositionChanged>();
    InitializeType<OrientationChanged>();
    InitializeType<CollisionEvent>();
    InitializeType<CollisionStarted>();
    InitializeType<CollisionPersisted>();
    InitializeType<CollisionEnded>();
    InitializeType<MouseEnter>();
    InitializeType<MouseExit>();

    InitializeType<Camera>();
    InitializeType<GraphicsSystem>();
    InitializeType<GraphicsView>();
    InitializeType<Model>();

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
