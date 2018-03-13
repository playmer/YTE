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

#include "YTE/Meta/ForwardDeclarations.hpp"

#include <qmenu.h>

namespace YTE
{
  class Composition;
}

namespace YTEditor
{

  class MainWindow;

  class GameObjectMenu : public QMenu
  {
  public:

    GameObjectMenu(MainWindow *aMainWindow);
    ~GameObjectMenu();

    YTE::Composition* MakeObject(std::string aName, std::string meshName);

  private:

    MainWindow * mMainWindow;

    QAction * MakeEmptyObjectAction();
    void CreateEmptyObject();

    QMenu * Make3DObjectMenu();
    void CreateCube();
    void CreateSphere();
    void CreateCylinder();
    void CreatePlane();

    QMenu * Make2DObjectMenu();
    void CreateSprite();

    QMenu * MakeLightMenu();
    void CreatePointLight();
    void CreateDirectionalLight();
    void CreateSpotlight();
    YTE::Composition* MakeLight(std::string lightType);

    QMenu * MakeAudioMenu();
    void CreateAudioEmitter();
    void CreateAudioListener();

    QMenu * MakeUIMenu();
    void CreateText();
    void CreateImage();
    void CreateButton();

    QAction * MakeParticleSystemAction();
    void CreateParticleSystem();

    QAction * MakeCameraAction();
    void CreateCamera();


  };

}
