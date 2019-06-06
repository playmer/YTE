/******************************************************************************/
/*!
\file   GameObjectMenu.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the game object menu in the menu bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/Light.hpp"
#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseListener.hpp"

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/YTELevelEditor/MenuBar/GameObjectMenu.hpp"

#include "YTEditor/YTELevelEditor/Physics/PhysicsHandler.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectItem.hpp"

#include "YTEditor/YTELevelEditor/Gizmo.hpp"
#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{
  GameObjectMenu::GameObjectMenu(Framework::MainWindow* aMainWindow)
    : Framework::Menu("Game Object", aMainWindow->GetWorkspace<YTELevelEditor>())
    , mObjectBrowser(aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<ObjectBrowser>())
    , mComponentBrowser(aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<ComponentBrowser>())
    , mComponentTree(mComponentBrowser->GetComponentTree())
  {
    AddAction<GameObjectMenu>("Empty Object", &GameObjectMenu::CreateEmptyObject, this);

    AddMenu(Make3DObjectMenu());
    AddMenu(Make2DObjectMenu());
    AddMenu(MakeLightMenu());
    AddMenu(MakeAudioMenu());

    AddAction<GameObjectMenu>("Particle Emitter", &GameObjectMenu::CreateParticleSystem, this);

    AddAction<GameObjectMenu>("Game Camera", &GameObjectMenu::CreateCamera, this);
  }

  void GameObjectMenu::CreateEmptyObject()
  {
    mObjectBrowser->AddObject("EmptyObject", "Empty");
  }

  Framework::Menu* GameObjectMenu::Make3DObjectMenu()
  {
    Menu *menu = new Menu("3D Object", mWorkspace);

    menu->AddAction<GameObjectMenu>("Cube", &GameObjectMenu::CreateCube, this);
    menu->AddAction<GameObjectMenu>("Sphere", &GameObjectMenu::CreateSphere, this);
    menu->AddAction<GameObjectMenu>("Cylinder", &GameObjectMenu::CreateCylinder, this);
    menu->AddAction<GameObjectMenu>("Plane", &GameObjectMenu::CreatePlane, this);

    return menu;
  }

  void GameObjectMenu::CreateCube()
  {
    YTE::Composition *cube = MakeObject("Cube", "cube.fbx");
    mComponentTree->LoadGameObject(cube);
  }

  void GameObjectMenu::CreateSphere()
  {
    YTE::Composition *sphere = MakeObject("Sphere", "sphere.fbx");
    mComponentTree->LoadGameObject(sphere);
  }

  void GameObjectMenu::CreateCylinder()
  {
    YTE::Composition *cylinder = MakeObject("Cylinder", "cylinder.fbx");
    mComponentTree->LoadGameObject(cylinder);
  }

  void GameObjectMenu::CreatePlane()
  {
    YTE::Composition *plane = MakeObject("Plane", "plane.fbx");
    mComponentTree->LoadGameObject(plane);
  }

  Framework::Menu* GameObjectMenu::Make2DObjectMenu()
  {
    Menu *menu = new Menu("2D Object", mWorkspace);

    menu->AddAction<GameObjectMenu>("Sprite", &GameObjectMenu::CreateSprite, this);

    return menu;
  }

  void GameObjectMenu::CreateSprite()
  {
    ObjectItem *item = mObjectBrowser->AddObject("Sprite", "Empty");
    YTE::Composition *obj = item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);
    
    YTE::BoundType* transform = YTE::Transform::GetStaticType();
    mComponentTree->AddComponent(transform);
    
    YTE::BoundType* spriteType = YTE::Sprite::GetStaticType();
    ComponentWidget *compWidget = mComponentTree->AddComponent(spriteType);
    

    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);
    levelEditor->GetPhysicsHandler()->Remove(obj);
    levelEditor->GetPhysicsHandler()->Add(obj);
    
    levelEditor->GetGizmo()->SnapToCurrentObject();

    mComponentTree->LoadGameObject(obj);
  }

  Framework::Menu* GameObjectMenu::MakeLightMenu()
  {
    auto menu = new Framework::Menu("Light", mWorkspace);

    menu->AddAction<GameObjectMenu>("Point Light", &GameObjectMenu::CreatePointLight, this);
    menu->AddAction<GameObjectMenu>("Directional Light", &GameObjectMenu::CreateDirectionalLight, this);
    menu->AddAction<GameObjectMenu>("Spotlight", &GameObjectMenu::CreateSpotlight, this);

    return menu;
  }

  Framework::Menu* GameObjectMenu::MakeAudioMenu()
  {
    Menu* menu = new Framework::Menu("Audio", mWorkspace);

    menu->AddAction<GameObjectMenu>("Audio Emitter", &GameObjectMenu::CreateAudioListener, this);
    menu->AddAction<GameObjectMenu>("Audio Listener", &GameObjectMenu::CreateAudioListener, this);

    return menu;
  }

  Framework::Menu* GameObjectMenu::MakeUIMenu()
  {
    auto menu = new Framework::Menu("UI", mWorkspace);

    menu->AddAction<GameObjectMenu>("Text", &GameObjectMenu::CreateText, this);
    menu->AddAction<GameObjectMenu>("Image", &GameObjectMenu::CreateImage, this);
    menu->AddAction<GameObjectMenu>("Button", &GameObjectMenu::CreateButton, this);

    return menu;
  }

  void GameObjectMenu::CreatePointLight()
  {
    YTE::Composition* pointLight = MakeLight("Point");
    mComponentTree->LoadGameObject(pointLight);
  }

  void GameObjectMenu::CreateDirectionalLight()
  {
    YTE::Composition* dirLight = MakeLight("Directional");
    mComponentTree->LoadGameObject(dirLight);
  }

  void GameObjectMenu::CreateSpotlight()
  {
    YTE::Composition* spotlight = MakeLight("Spot");
    mComponentTree->LoadGameObject(spotlight);
  }

  YTE::Composition* GameObjectMenu::MakeLight(std::string lightType)
  {
    std::string objName = lightType + " Light";
    ObjectItem *item = mObjectBrowser->AddObject(objName.c_str(), "Empty");
    YTE::Composition *obj = item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);

    YTE::BoundType* transform = YTE::Transform::GetStaticType();
    mComponentTree->AddComponent(transform);

    YTE::BoundType* light = YTE::Light::GetStaticType();
    ComponentWidget *compWidget = mComponentTree->AddComponent(light);

    YTE::Light *lightComponent = static_cast<YTE::Light*>(compWidget->GetEngineComponent());
    lightComponent->SetLightType(lightType);

    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);
    levelEditor->GetPhysicsHandler()->Remove(obj);
    levelEditor->GetPhysicsHandler()->Add(obj);

    levelEditor->GetGizmo()->SnapToCurrentObject();

    return obj;
  }

  void GameObjectMenu::CreateAudioEmitter()
  {
    ObjectItem *item = mObjectBrowser->AddObject("Audio Emitter", "Empty");
    YTE::Composition *obj = item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);

    YTE::BoundType* transform = YTE::Transform::GetStaticType();
    mComponentTree->AddComponent(transform);

    YTE::BoundType* orientation = YTE::Orientation::GetStaticType();
    mComponentTree->AddComponent(orientation);

    YTE::BoundType* emitterType = YTE::WWiseEmitter::GetStaticType();
    mComponentTree->AddComponent(emitterType);

    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);
    levelEditor->GetPhysicsHandler()->Remove(obj);
    levelEditor->GetPhysicsHandler()->Add(obj);

    levelEditor->GetGizmo()->SnapToCurrentObject();
    mComponentTree->LoadGameObject(obj);
  }

  void GameObjectMenu::CreateAudioListener()
  {
    ObjectItem *item = mObjectBrowser->AddObject("Audio Listener", "Empty");
    YTE::Composition *obj = item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);

    YTE::BoundType* transform = YTE::Transform::GetStaticType();
    mComponentTree->AddComponent(transform);

    YTE::BoundType* orientation = YTE::Orientation::GetStaticType();
    mComponentTree->AddComponent(orientation);

    YTE::BoundType* listenerType = YTE::WWiseListener::GetStaticType();
    mComponentTree->AddComponent(listenerType);

    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);
    levelEditor->GetPhysicsHandler()->Remove(obj);
    levelEditor->GetPhysicsHandler()->Add(obj);

    levelEditor->GetGizmo()->SnapToCurrentObject();
    mComponentTree->LoadGameObject(obj);
  }

  void GameObjectMenu::CreateText()
  {
  }

  void GameObjectMenu::CreateImage()
  {
  }

  void GameObjectMenu::CreateButton()
  {
  }


  void GameObjectMenu::CreateParticleSystem()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);
    ObjectBrowser *objectBrowser = levelEditor->GetWidget<ObjectBrowser>();
    ObjectItem *item = objectBrowser->AddObject("Particle Emitter", "Empty");
    YTE::Composition *obj = item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);

    YTE::BoundType* transform = YTE::Transform::GetStaticType();
    mComponentTree->AddComponent(transform);

    YTE::BoundType* particleType = YTE::ParticleEmitter::GetStaticType();
    mComponentTree->AddComponent(particleType);

    levelEditor->GetPhysicsHandler()->Remove(obj);
    levelEditor->GetPhysicsHandler()->Add(obj);

    levelEditor->GetGizmo()->SnapToCurrentObject();
    mComponentTree->LoadGameObject(obj);
  }

  void GameObjectMenu::CreateCamera()
  {
    ObjectItem *item = mObjectBrowser->AddObject("Particle Emitter", "Empty");
    YTE::Composition *obj = item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);

    YTE::BoundType* transform = YTE::Transform::GetStaticType();
    mComponentTree->AddComponent(transform);

    YTE::BoundType* orientation = YTE::Orientation::GetStaticType();
    mComponentTree->AddComponent(orientation);

    YTE::BoundType* particleType = YTE::Camera::GetStaticType();
    mComponentTree->AddComponent(particleType);

    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);
    levelEditor->GetPhysicsHandler()->Remove(obj);
    levelEditor->GetPhysicsHandler()->Add(obj);

    levelEditor->GetGizmo()->SnapToCurrentObject();
    mComponentTree->LoadGameObject(obj);
  }

  YTE::Composition* GameObjectMenu::MakeObject(std::string aName, std::string meshName)
  {
    ObjectItem *item = mObjectBrowser->AddObject(aName.c_str(), "Empty");
    YTE::Composition *obj= item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);

    // add transform
    YTE::BoundType* transform = YTE::TypeId<YTE::Transform>();
    mComponentTree->AddComponent(transform);

    // add model
    YTE::BoundType* modelType = YTE::TypeId<YTE::Model>();
    ComponentWidget *compWidget = mComponentTree->AddComponent(modelType);

    YTE::Model *modelComponent = static_cast<YTE::Model*>(compWidget->GetEngineComponent());
    modelComponent->SetMesh(meshName);

    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);
    levelEditor->GetPhysicsHandler()->Remove(obj);
    levelEditor->GetPhysicsHandler()->Add(obj);

    levelEditor->GetGizmo()->SnapToCurrentObject();

    return obj;
  }
}
