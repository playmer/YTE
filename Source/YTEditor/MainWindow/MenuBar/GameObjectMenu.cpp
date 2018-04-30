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

#include "YTEditor/MainWindow/Gizmo.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/MenuBar/GameObjectMenu.hpp"
#include "YTEditor/MainWindow/SubWindows/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/MainWindow/SubWindows/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/MainWindow/SubWindows/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/MainWindow/SubWindows/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/MainWindow/SubWindows/ObjectBrowser/ObjectItem.hpp"


namespace YTEditor
{
  GameObjectMenu::GameObjectMenu(MainWindow *aMainWindow)
    : Menu("Game Object", aMainWindow)
    , mComponentBrowser(&aMainWindow->GetComponentBrowser())
    , mComponentTree(mComponentBrowser->GetComponentTree())
  {
    AddAction<GameObjectMenu>("Empty Object", &GameObjectMenu::CreateEmptyObject);

    AddMenu(Make3DObjectMenu());
    AddMenu(Make2DObjectMenu());
    AddMenu(MakeLightMenu());
    AddMenu(MakeAudioMenu());

    AddAction<GameObjectMenu>("Particle Emitter", &GameObjectMenu::CreateParticleSystem);

    AddAction<GameObjectMenu>("Game Camera", &GameObjectMenu::CreateCamera);
  }

  void GameObjectMenu::CreateEmptyObject()
  {
    mObjectBrowser->AddObject("EmptyObject", "Empty");
  }

  Menu* GameObjectMenu::Make3DObjectMenu()
  {
    Menu *menu = new Menu("3D Object", mMainWindow);

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

  Menu* GameObjectMenu::Make2DObjectMenu()
  {
    Menu *menu = new Menu("2D Object", mMainWindow);

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
    
    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);
    
    mMainWindow->GetGizmo()->SnapToCurrentObject();

    mComponentTree->LoadGameObject(obj);
  }

  Menu* GameObjectMenu::MakeLightMenu()
  {
    Menu *menu = new Menu("Light", mMainWindow);

    menu->AddAction<GameObjectMenu>("Point Light", &GameObjectMenu::CreatePointLight, this);
    menu->AddAction<GameObjectMenu>("Directional Light", &GameObjectMenu::CreateDirectionalLight, this);
    menu->AddAction<GameObjectMenu>("Spotlight", &GameObjectMenu::CreateSpotlight, this);

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

    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);

    mMainWindow->GetGizmo()->SnapToCurrentObject();

    return obj;
  }

  Menu* GameObjectMenu::MakeAudioMenu()
  {
    Menu *menu = new Menu("Audio", mMainWindow);

    menu->AddAction<GameObjectMenu>("Audio Emitter", &GameObjectMenu::CreateAudioListener);
    menu->AddAction<GameObjectMenu>("Audio Listener", &GameObjectMenu::CreateAudioListener);

    return menu;
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

    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);

    mMainWindow->GetGizmo()->SnapToCurrentObject();
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

    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);

    mMainWindow->GetGizmo()->SnapToCurrentObject();
    mComponentTree->LoadGameObject(obj);
  }

  Menu* GameObjectMenu::MakeUIMenu()
  {
    Menu *menu = new Menu("UI", mMainWindow);

    menu->AddAction<GameObjectMenu>("Text", &GameObjectMenu::CreateText);
    menu->AddAction<GameObjectMenu>("Image", &GameObjectMenu::CreateImage);
    menu->AddAction<GameObjectMenu>("Button", &GameObjectMenu::CreateButton);

    return menu;
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
    ObjectItem *item = mMainWindow->GetObjectBrowser().AddObject("Particle Emitter", "Empty");
    YTE::Composition *obj = item->GetEngineObject();
    mObjectBrowser->MoveToFrontOfCamera(obj);

    YTE::BoundType* transform = YTE::Transform::GetStaticType();
    mComponentTree->AddComponent(transform);

    YTE::BoundType* particleType = YTE::ParticleEmitter::GetStaticType();
    mComponentTree->AddComponent(particleType);

    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);

    mMainWindow->GetGizmo()->SnapToCurrentObject();
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

    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);

    mMainWindow->GetGizmo()->SnapToCurrentObject();
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

    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);

    mMainWindow->GetGizmo()->SnapToCurrentObject();

    return obj;
  }
}
