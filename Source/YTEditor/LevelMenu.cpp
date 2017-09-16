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

#include "LevelMenu.hpp"
#include "YTEditorMainWindow.hpp"
#include "ObjectBrowser.hpp"
#include "ObjectItem.hpp"
#include "ComponentBrowser.hpp"
#include "ComponentTree.hpp"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Utilities.hpp"


LevelMenu::LevelMenu(YTEditorMainWindow *aMainWindow)
  : QMenu("Level"), 
    mMainWindow(aMainWindow)
{
  addMenu(MakeCurrentLevelMenu());
  addMenu(MakeSpaceMenu());
  addMenu(MakeEngineMenu());
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
  // Get all the compositions on the engine
  YTE::CompositionMap *engineMap = mMainWindow->GetRunningEngine()->GetCompositions();

  // iterator to the main session space
  auto it_lvl = engineMap->begin();

  // Get the space that represents the main session
  YTE::Space *lvl = static_cast<YTE::Space*>(it_lvl->second.get());

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

    // Store the name and composition pointer in the object browser
    ObjectItem *topItem = mMainWindow->GetObjectBrowser().AddExistingComposition(objName.c_str(), 
                                                                                  component);

    mMainWindow->GetObjectBrowser().LoadAllChildObjects(cmp.second.get(), topItem);
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
  // Get all the compositions on the engine
  YTE::CompositionMap *engineMap = mMainWindow->GetRunningEngine()->GetCompositions();

  // iterator to the main session space
  auto it_lvl = engineMap->begin();

  // Get the space that represents the main session
  YTE::Space *lvl = static_cast<YTE::Space*>(it_lvl->second.get());

  mMainWindow->GetObjectBrowser().SelectNoItem();

  mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(lvl);
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

  mMainWindow->GetObjectBrowser().SelectNoItem();

  mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(engine);
}
