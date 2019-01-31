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

#include "YTEditor/YTELevelEditor/MainWindow.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/LevelMenu.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectItem.hpp"


namespace YTEditor
{

  LevelMenu::LevelMenu(MainWindow *aMainWindow)
    : Menu("Level", aMainWindow)
    , mObjectBrowser(aMainWindow->GetWidget<ObjectBrowser>())
    , mComponentTree(aMainWindow->GetWidget<ComponentBrowser>()->GetComponentTree())
  {
    AddAction<LevelMenu>("Reload Level", &LevelMenu::SelectEngine, this);
    AddAction<LevelMenu>("Select Space", &LevelMenu::SelectSpace, this);
    AddAction<LevelMenu>("Select Engine", &LevelMenu::SelectEngine, this);
    AddAction<LevelMenu>("Select Camera", &LevelMenu::SelectCamera, this);
    
    AddMenu(MakeSetLightingMenu());
  }

  void LevelMenu::ReloadCurrentLevel()
  {
    // Get the space that represents the main session
    YTE::Space *lvl = mMainWindow->GetEditingLevel();

    //////////////////////////////////////////////////////////////////////////////
    // Clear the items (names and composition pointers) from the current object browser
    mObjectBrowser->ClearObjectList();

    // Set the name to the new level
    mObjectBrowser->setHeaderLabel(lvl->GetName().c_str());
    /////////////////////////////////////////////////////////////////////////////

    // Iterate through all the objects in the map / on the level
    for (auto const& [name, composition] : lvl->GetCompositions())
    {
      // Get the name of the object
      auto objName = composition->GetName();

      // Store the name and composition pointer in the object browser
      ObjectItem *topItem = mObjectBrowser->AddExistingComposition(objName.c_str(), composition.get());
    }
  }

  void LevelMenu::SelectSpace()
  {
    // Get the space that represents the main session
    YTE::Space *lvl = mMainWindow->GetEditingLevel();

    auto objItem = mObjectBrowser->AddExistingComposition(mMainWindow->GetRunningLevelName().c_str(), lvl);

    mObjectBrowser->setCurrentItem(objItem);

    mComponentTree->LoadGameObject(lvl);
  }

  void LevelMenu::SelectCamera()
  {
    auto view = mMainWindow->GetEditingLevel()->GetComponent<YTE::GraphicsView>();
    auto cameraComponent = view->GetActiveCamera();
    auto cameraObject = cameraComponent->GetOwner();

    mComponentTree->LoadGameObject(cameraObject);
  }

  void LevelMenu::SelectEngine()
  {
    // Get all the compositions on the engine
    YTE::Composition *engine = mMainWindow->GetRunningEngine();

    mObjectBrowser->SelectNoItem();

    mComponentTree->LoadGameObject(engine);
  }

  Menu* LevelMenu::MakeSetLightingMenu()
  {
    Menu *menu = new Menu("Lighting", mMainWindow);

    menu->AddAction<LevelMenu>("All Lights On", &LevelMenu::TurnLightsOn, this);
    menu->AddAction<LevelMenu>("All Lights Off", &LevelMenu::TurnLightsOff, this);

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
