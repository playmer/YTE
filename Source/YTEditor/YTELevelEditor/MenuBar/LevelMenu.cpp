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

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/YTELevelEditor/MenuBar/LevelMenu.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectItem.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{

  LevelMenu::LevelMenu(Framework::MainWindow *aMainWindow)
    : Menu("Level", aMainWindow->GetWorkspace<YTELevelEditor>())
    , mObjectBrowser(aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<ObjectBrowser>())
    , mComponentTree(aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<ComponentBrowser>()->GetComponentTree())
  {
    AddAction<LevelMenu>("Reload Level", &LevelMenu::SelectEngine, this);
    AddAction<LevelMenu>("Select Space", &LevelMenu::SelectSpace, this);
    AddAction<LevelMenu>("Select Engine", &LevelMenu::SelectEngine, this);
    AddAction<LevelMenu>("Select Camera", &LevelMenu::SelectCamera, this);
    
    AddMenu(MakeSetLightingMenu());
  }

  void LevelMenu::ReloadCurrentLevel()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // Get the space that represents the main session
    YTE::Space *lvl = levelEditor->GetEditingLevel();

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
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // Get the space that represents the main session
    YTE::Space *lvl = levelEditor->GetEditingLevel();

    auto objItem = mObjectBrowser->AddExistingComposition(levelEditor->GetRunningLevelName().c_str(), lvl);

    mObjectBrowser->setCurrentItem(objItem);

    mComponentTree->LoadGameObject(lvl);
  }

  void LevelMenu::SelectCamera()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    auto view = levelEditor->GetEditingLevel()->GetComponent<YTE::GraphicsView>();
    auto cameraComponent = view->GetActiveCamera();
    auto cameraObject = cameraComponent->GetOwner();

    mComponentTree->LoadGameObject(cameraObject);
  }

  void LevelMenu::SelectEngine()
  {
    // Get all the compositions on the engine
    YTE::Composition *engine = static_cast<YTELevelEditor*>(mWorkspace)->GetRunningEngine();

    mObjectBrowser->SelectNoItem();

    mComponentTree->LoadGameObject(engine);
  }

  Framework::Menu* LevelMenu::MakeSetLightingMenu()
  {
    auto menu = new Framework::Menu("Lighting", mWorkspace);

    menu->AddAction<LevelMenu>("All Lights On", &LevelMenu::TurnLightsOn, this);
    menu->AddAction<LevelMenu>("All Lights Off", &LevelMenu::TurnLightsOff, this);

    return menu;
  }

  void LevelMenu::TurnLightsOn()
  {
    auto engine = static_cast<YTELevelEditor*>(mWorkspace)->GetRunningEngine();

    auto graphics = engine->GetComponent<YTE::GraphicsSystem>();

    auto renderer = graphics->GetRenderer();

    renderer->SetLights(true);
  }

  void LevelMenu::TurnLightsOff()
  {
    auto engine = static_cast<YTELevelEditor*>(mWorkspace)->GetRunningEngine();

    auto graphics = engine->GetComponent<YTE::GraphicsSystem>();

    auto renderer = graphics->GetRenderer();

    renderer->SetLights(false);
  }
}
