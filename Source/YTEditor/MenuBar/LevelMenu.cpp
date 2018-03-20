/******************************************************************************/
/*!
\file   LevelMenu.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the level menu in the menu bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/


#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Camera.hpp"

#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MenuBar/LevelMenu.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/ObjectBrowser/ObjectItem.hpp"


namespace YTEditor
{

  LevelMenu::LevelMenu(MainWindow *aMainWindow)
    : QMenu("Level"),
    mMainWindow(aMainWindow)
  {
    addMenu(MakeCurrentLevelMenu());
    addMenu(MakeSpaceMenu());
    addMenu(MakeEngineMenu());
    addMenu(MakeSetLightingMenu());
    addAction(MakeSelectCameraAct());
  }

  LevelMenu::~LevelMenu()
  {
  }

  QMenu* LevelMenu::MakeCurrentLevelMenu()
  {
    QMenu *menu = new QMenu("Current Level");

    QAction *reloadAct = new QAction("Reload");
    menu->addAction(reloadAct);
    connect(reloadAct, &QAction::triggered, this, &LevelMenu::ReloadCurrentLevel);

    return menu;
  }

  void LevelMenu::ReloadCurrentLevel()
  {
    // Get the space that represents the main session
    YTE::Space *lvl = mMainWindow->GetEditingLevel();

    //////////////////////////////////////////////////////////////////////////////
    // Clear the items (names and composition pointers) from the current object browser
    mMainWindow->GetObjectBrowser().ClearObjectBrowser();

    // Set the name to the new level
    mMainWindow->GetObjectBrowser().setHeaderLabel(lvl->GetName().c_str());
    /////////////////////////////////////////////////////////////////////////////

    // Get all compositions on the main session (should be levels)
    YTE::CompositionMap *objMap = lvl->GetCompositions();

    // Iterate through all the objects in the map / on the level
    for (auto& cmp : *objMap)
    {
      auto component = cmp.second.get();

      // Get the name of the object
      auto objName = component->GetName();

      auto &objectBrowser = mMainWindow->GetObjectBrowser();

      // Store the name and composition pointer in the object browser
      ObjectItem *topItem = objectBrowser.AddExistingComposition(objName.c_str(),
                                                                 component);

      if (topItem)
      {
        objectBrowser.LoadAllChildObjects(cmp.second.get(), topItem);
      }

    }
  }

  QMenu* LevelMenu::MakeSpaceMenu()
  {
    QMenu *menu = new QMenu("Space");

    QAction *selectAct = new QAction("Select");
    menu->addAction(selectAct);
    connect(selectAct, &QAction::triggered, this, &LevelMenu::SelectSpace);

    return menu;
  }

  void LevelMenu::SelectSpace()
  {
    // Get the space that represents the main session
    YTE::Space *lvl = mMainWindow->GetEditingLevel();

    auto &objectBrowser = mMainWindow->GetObjectBrowser();
    auto objItem = objectBrowser.AddExistingComposition(mMainWindow->GetRunningLevelName().c_str(), lvl);

    objectBrowser.setCurrentItem(objItem);

    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(lvl);
  }

  QAction* LevelMenu::MakeSelectCameraAct()
  {
    QAction *action = new QAction("Select Camera");
    connect(action, &QAction::triggered, this, &LevelMenu::SelectCamera);
    return action;
  }

  void LevelMenu::SelectCamera()
  {
    auto view = mMainWindow->GetEditingLevel()->GetComponent<YTE::GraphicsView>();
    auto cameraComponent = view->GetActiveCamera();
    auto cameraObject = cameraComponent->GetOwner();

    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(cameraObject);
  }

  QMenu* LevelMenu::MakeEngineMenu()
  {
    QMenu *menu = new QMenu("Engine");

    QAction *selectAct = new QAction("Select");
    menu->addAction(selectAct);
    connect(selectAct, &QAction::triggered, this, &LevelMenu::SelectEngine);

    return menu;
  }

  void LevelMenu::SelectEngine()
  {
    // Get all the compositions on the engine
    YTE::Composition *engine = mMainWindow->GetRunningEngine();

    auto &objBrowser = mMainWindow->GetObjectBrowser();
    objBrowser.SelectNoItem();

    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(engine);
  }

  QMenu* LevelMenu::MakeSetLightingMenu()
  {
    QMenu *menu = new QMenu("Lighting");

    QAction *lightsOn = new QAction("All Lights On");
    menu->addAction(lightsOn);
    connect(lightsOn, &QAction::triggered, this, &LevelMenu::TurnLightsOn);

    QAction *lightsOff = new QAction("All Lights Off");
    menu->addAction(lightsOff);
    connect(lightsOff, &QAction::triggered, this, &LevelMenu::TurnLightsOff);

    return menu;
  }

  void LevelMenu::TurnLightsOn()
  {
    auto engine = mMainWindow->GetRunningEngine();

    auto graphics = engine->GetComponent<YTE::GraphicsSystem>();

    auto renderer = graphics->GetRenderer();

    renderer->SetLights(true);
  }

  void LevelMenu::TurnLightsOff()
  {
    auto engine = mMainWindow->GetRunningEngine();

    auto graphics = engine->GetComponent<YTE::GraphicsSystem>();

    auto renderer = graphics->GetRenderer();

    renderer->SetLights(false);
  }

}
