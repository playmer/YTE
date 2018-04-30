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

#include "YTEditor/MainWindow/MenuBar/Menu.hpp"

namespace YTE
{
  class Composition;
}

namespace YTEditor
{

  class MainWindow;

  class GameObjectMenu : public Menu
  {
  public:
    GameObjectMenu(MainWindow *aMainWindow);

    YTE::Composition* MakeObject(std::string aName, std::string meshName);

  private:
    
    ObjectBrowser *mObjectBrowser;
    ComponentBrowser *mComponentBrowser;
    ComponentTree *mComponentTree;

    void CreateEmptyObject();

    Menu* Make3DObjectMenu();
    void CreateCube();
    void CreateSphere();
    void CreateCylinder();
    void CreatePlane();

    Menu* Make2DObjectMenu();
    void CreateSprite();

    Menu* MakeLightMenu();
    void CreatePointLight();
    void CreateDirectionalLight();
    void CreateSpotlight();
    YTE::Composition* MakeLight(std::string lightType);

    Menu* MakeAudioMenu();
    void CreateAudioEmitter();
    void CreateAudioListener();

    Menu* MakeUIMenu();
    void CreateText();
    void CreateImage();
    void CreateButton();

    QAction * MakeParticleSystemAction();
    void CreateParticleSystem();

    QAction * MakeCameraAction();
    void CreateCamera();

  };

}
