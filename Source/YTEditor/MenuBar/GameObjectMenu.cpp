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

#include "../MainWindow/YTEditorMainWindow.hpp"
#include "../ObjectBrowser/ObjectBrowser.hpp"

#include "GameObjectMenu.hpp"



GameObjectMenu::GameObjectMenu(YTEditorMainWindow * aMainWindow) 
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

QAction * GameObjectMenu::MakeEmptyObjectAction()
{
  QAction * emptyObjAct = new QAction("Empty Object");
  connect(emptyObjAct, &QAction::triggered, this, &GameObjectMenu::CreateEmptyObject);
  return emptyObjAct;
}

void GameObjectMenu::CreateEmptyObject()
{
  mMainWindow->GetObjectBrowser().AddObject("EmptyObject", "Empty");
}

QMenu * GameObjectMenu::Make3DObjectMenu()
{
  QMenu * menu = new QMenu("3D Object");

  QAction * cubeAct = new QAction("Cube");
  menu->addAction(cubeAct);
  connect(cubeAct, &QAction::triggered, this, &GameObjectMenu::CreateCube);

  QAction * sphereAct = new QAction("Sphere");
  menu->addAction(sphereAct);
  connect(sphereAct, &QAction::triggered, this, &GameObjectMenu::CreateSphere);

  QAction * planeAct = new QAction("Plane");
  menu->addAction(planeAct);
  connect(planeAct, &QAction::triggered, this, &GameObjectMenu::CreatePlane);

  return menu;
}

void GameObjectMenu::CreateCube()
{
}

void GameObjectMenu::CreateSphere()
{
}

void GameObjectMenu::CreatePlane()
{
}

QMenu * GameObjectMenu::Make2DObjectMenu()
{
  QMenu * menu = new QMenu("2D Object");

  QAction * spriteAct = new QAction("Sprite");
  menu->addAction(spriteAct);
  connect(spriteAct, &QAction::triggered, this, &GameObjectMenu::CreateSprite);

  return menu;
}

void GameObjectMenu::CreateSprite()
{
}

QMenu * GameObjectMenu::MakeLightMenu()
{
  QMenu * menu = new QMenu("Light");

  QAction * pointLightAct = new QAction("Point Light");
  menu->addAction(pointLightAct);
  connect(pointLightAct, &QAction::triggered, this, &GameObjectMenu::CreatePointLight);

  QAction * directionalLightAct = new QAction("Directional Light");
  menu->addAction(directionalLightAct);
  connect(directionalLightAct, &QAction::triggered, this, &GameObjectMenu::CreateDirectionalLight);

  QAction * areaLightAct = new QAction("Area Light");
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

QMenu * GameObjectMenu::MakeAudioMenu()
{
  QMenu * menu = new QMenu("Audio");

  QAction * audioSourceAct = new QAction("Audio Source");
  menu->addAction(audioSourceAct);
  connect(audioSourceAct, &QAction::triggered, this, &GameObjectMenu::CreateAudioSource);

  return menu;
}

void GameObjectMenu::CreateAudioSource()
{
}

QMenu * GameObjectMenu::MakeUIMenu()
{
  QMenu * menu = new QMenu("UI");

  QAction * textAct = new QAction("Text");
  menu->addAction(textAct);
  connect(textAct, &QAction::triggered, this, &GameObjectMenu::CreateText);

  QAction * imageAct = new QAction("Image");
  menu->addAction(imageAct);
  connect(imageAct, &QAction::triggered, this, &GameObjectMenu::CreateImage);

  QAction * buttonAct = new QAction("Button");
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

QAction * GameObjectMenu::MakeParticleSystemAction()
{
  QAction * particleAct = new QAction("Particle System");
  connect(particleAct, &QAction::triggered, this, &GameObjectMenu::CreateParticleSystem);
  return particleAct;
}

void GameObjectMenu::CreateParticleSystem()
{
}

QAction * GameObjectMenu::MakeCameraAction()
{
  QAction * cameraAct = new QAction("Camera");
  connect(cameraAct, &QAction::triggered, this, &GameObjectMenu::CreateCamera);
  return cameraAct;
}

void GameObjectMenu::CreateCamera()
{
}