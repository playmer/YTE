/******************************************************************************/
/*!
\file   GameObjectMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The menu bar drop down for creating new types of game objects.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qmenu.h>

class YTEditorMainWindow;

class GameObjectMenu : public QMenu
{
public:

  GameObjectMenu(YTEditorMainWindow *aMainWindow);
  ~GameObjectMenu();

private:

  YTEditorMainWindow * mMainWindow;

  QAction * MakeEmptyObjectAction();
  void CreateEmptyObject();

  QMenu * Make3DObjectMenu();
  void CreateCube();
  void CreateSphere();
  void CreatePlane();

  QMenu * Make2DObjectMenu();
  void CreateSprite();

  QMenu * MakeLightMenu();
  void CreatePointLight();
  void CreateDirectionalLight();
  void CreateAreaLight();

  QMenu * MakeAudioMenu();
  void CreateAudioSource();

  QMenu * MakeUIMenu();
  void CreateText();
  void CreateImage();
  void CreateButton();

  QAction * MakeParticleSystemAction();
  void CreateParticleSystem();

  QAction * MakeCameraAction();
  void CreateCamera();

};