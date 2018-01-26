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

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/MenuBar/GameObjectMenu.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/ComponentBrowser/ComponentWidget.hpp"


namespace YTEditor
{

  GameObjectMenu::GameObjectMenu(MainWindow * aMainWindow)
    : QMenu("Game Object"), mMainWindow(aMainWindow)
  {
    addAction(MakeEmptyObjectAction());
    addMenu(Make3DObjectMenu());
    addMenu(Make2DObjectMenu());
    addMenu(MakeLightMenu());
    addMenu(MakeAudioMenu());
    addMenu(MakeUIMenu());
    addAction(MakeParticleSystemAction());
    addAction(MakeCameraAction());
  }

  GameObjectMenu::~GameObjectMenu()
  {
  }

  QAction* GameObjectMenu::MakeEmptyObjectAction()
  {
    QAction *emptyObjAct = new QAction("Empty Object");
    connect(emptyObjAct, &QAction::triggered, this, &GameObjectMenu::CreateEmptyObject);
    return emptyObjAct;
  }

  void GameObjectMenu::CreateEmptyObject()
  {
    mMainWindow->GetObjectBrowser().AddObject("EmptyObject", "Empty");
  }

  QMenu* GameObjectMenu::Make3DObjectMenu()
  {
    QMenu *menu = new QMenu("3D Object");

    QAction *cubeAct = new QAction("Cube");
    menu->addAction(cubeAct);
    connect(cubeAct, &QAction::triggered, this, &GameObjectMenu::CreateCube);

    QAction *sphereAct = new QAction("Sphere");
    menu->addAction(sphereAct);
    connect(sphereAct, &QAction::triggered, this, &GameObjectMenu::CreateSphere);

    QAction *cylinderAct = new QAction("Cylinder");
    menu->addAction(cylinderAct);
    connect(cylinderAct, &QAction::triggered, this, &GameObjectMenu::CreateCylinder);

    QAction *planeAct = new QAction("Plane");
    menu->addAction(planeAct);
    connect(planeAct, &QAction::triggered, this, &GameObjectMenu::CreatePlane);

    return menu;
  }

  void GameObjectMenu::CreateCube()
  {
    YTE::Composition *cube = MakeObject("Cube", "cube.fbx");
    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(cube);
  }

  void GameObjectMenu::CreateSphere()
  {
    YTE::Composition *sphere = MakeObject("Sphere", "sphere.fbx");
    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(sphere);
  }

  void GameObjectMenu::CreateCylinder()
  {
    YTE::Composition *cylinder = MakeObject("Cylinder", "cylinder.fbx");
    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(cylinder);
  }

  void GameObjectMenu::CreatePlane()
  {
    YTE::Composition *plane = MakeObject("Plane", "plane.fbx");
    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(plane);
  }

  QMenu* GameObjectMenu::Make2DObjectMenu()
  {
    QMenu *menu = new QMenu("2D Object");

    QAction * spriteAct = new QAction("Sprite");
    menu->addAction(spriteAct);
    connect(spriteAct, &QAction::triggered, this, &GameObjectMenu::CreateSprite);

    return menu;
  }

  void GameObjectMenu::CreateSprite()
  {
  }

  QMenu* GameObjectMenu::MakeLightMenu()
  {
    QMenu * menu = new QMenu("Light");

    QAction *pointLightAct = new QAction("Point Light");
    menu->addAction(pointLightAct);
    connect(pointLightAct, &QAction::triggered, this, &GameObjectMenu::CreatePointLight);

    QAction *directionalLightAct = new QAction("Directional Light");
    menu->addAction(directionalLightAct);
    connect(directionalLightAct, &QAction::triggered, this, &GameObjectMenu::CreateDirectionalLight);

    QAction *areaLightAct = new QAction("Area Light");
    menu->addAction(areaLightAct);
    connect(areaLightAct, &QAction::triggered, this, &GameObjectMenu::CreateAreaLight);

    return menu;
  }

  void GameObjectMenu::CreatePointLight()
  {
  }

  void GameObjectMenu::CreateDirectionalLight()
  {
  }

  void GameObjectMenu::CreateAreaLight()
  {
  }

  QMenu* GameObjectMenu::MakeAudioMenu()
  {
    QMenu *menu = new QMenu("Audio");

    QAction *audioSourceAct = new QAction("Audio Source");
    menu->addAction(audioSourceAct);
    connect(audioSourceAct, &QAction::triggered, this, &GameObjectMenu::CreateAudioSource);

    return menu;
  }

  void GameObjectMenu::CreateAudioSource()
  {
  }

  QMenu* GameObjectMenu::MakeUIMenu()
  {
    QMenu * menu = new QMenu("UI");

    QAction *textAct = new QAction("Text");
    menu->addAction(textAct);
    connect(textAct, &QAction::triggered, this, &GameObjectMenu::CreateText);

    QAction *imageAct = new QAction("Image");
    menu->addAction(imageAct);
    connect(imageAct, &QAction::triggered, this, &GameObjectMenu::CreateImage);

    QAction *buttonAct = new QAction("Button");
    menu->addAction(buttonAct);
    connect(buttonAct, &QAction::triggered, this, &GameObjectMenu::CreateButton);

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

  QAction* GameObjectMenu::MakeParticleSystemAction()
  {
    QAction * particleAct = new QAction("Particle System");
    connect(particleAct, &QAction::triggered, this, &GameObjectMenu::CreateParticleSystem);
    return particleAct;
  }

  void GameObjectMenu::CreateParticleSystem()
  {
  }

  QAction* GameObjectMenu::MakeCameraAction()
  {
    QAction *cameraAct = new QAction("Camera");
    connect(cameraAct, &QAction::triggered, this, &GameObjectMenu::CreateCamera);
    return cameraAct;
  }

  void GameObjectMenu::CreateCamera()
  {
  }

  YTE::Composition* GameObjectMenu::MakeObject(std::string aName, std::string meshName)
  {
    ObjectItem *item = mMainWindow->GetObjectBrowser().AddObject(aName.c_str(), "Empty");
    YTE::Composition *obj= item->GetEngineObject();

    ComponentTree *compTree = mMainWindow->GetComponentBrowser().GetComponentTree();

    // add transform
    YTE::BoundType* transform = YTE::Type::GetGlobalType("Transform");
    compTree->AddComponent(transform);

    // add model
    YTE::BoundType* modelType = YTE::Type::GetGlobalType("Model");
    ComponentWidget *compWidget = compTree->AddComponent(modelType);

    YTE::Model *modelComponent = static_cast<YTE::Model*>(compWidget->GetEngineComponent());

    modelComponent->SetMesh(meshName);
    mMainWindow->GetPhysicsHandler().Remove(obj);
    mMainWindow->GetPhysicsHandler().Add(obj);

    mMainWindow->GetGizmo()->SnapToCurrentObject();

    return obj;
  }

}
