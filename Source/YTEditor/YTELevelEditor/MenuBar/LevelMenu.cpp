#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Camera.hpp"

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/YTELevelEditor/MenuBar/LevelMenu.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/CompositionBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/ObjectItem.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{

  LevelMenu::LevelMenu(Framework::MainWindow *aMainWindow)
    : Menu("Level", aMainWindow->GetWorkspace<YTELevelEditor>())
    , mObjectBrowser(aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<CompositionBrowser>())
    , mComponentTree(aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<ComponentBrowser>()->GetComponentTree())
  {
    //AddAction<LevelMenu>("Reload Level", &LevelMenu::ReloadCurrentLevel, this);
    AddAction<LevelMenu>("Select Space", &LevelMenu::SelectSpace, this);
    //AddAction<LevelMenu>("Select Engine", &LevelMenu::SelectEngine, this);
    AddAction<LevelMenu>("Select Camera", &LevelMenu::SelectCamera, this);
    
    AddMenu(MakeSetLightingMenu());
  }

  void LevelMenu::ReloadCurrentLevel()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // Get the space that represents the main session
    YTE::Space* space = levelEditor->GetEditingLevel();

    // Clear the items (names and composition pointers) from the current object browser
    mObjectBrowser->ClearObjectList();

    // Reload the level
    mObjectBrowser->setHeaderLabel(space->GetName().c_str());
    mObjectBrowser->AddTreeItem(space->GetName().c_str(), space, 0, false);
    auto spaceItem = mObjectBrowser->topLevelItem(0);
    mObjectBrowser->setCurrentItem(spaceItem);
    mObjectBrowser->setItemExpanded(spaceItem, true);
  }

  void LevelMenu::SelectSpace()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    auto spaceItem = mObjectBrowser->FindItemByComposition(levelEditor->GetEditingLevel());
    mObjectBrowser->setCurrentItem(spaceItem);
    mObjectBrowser->setItemExpanded(spaceItem, true);
  }

  void LevelMenu::SelectCamera()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    auto view = levelEditor->GetEditingLevel()->GetComponent<YTE::GraphicsView>();
    auto cameraComponent = view->GetActiveCamera();
    auto cameraObject = cameraComponent->GetOwner();

    auto cameraItem = mObjectBrowser->FindItemByComposition(cameraObject);
    mObjectBrowser->setCurrentItem(cameraItem);
    mObjectBrowser->setItemExpanded(cameraItem, true);
  }

  //void LevelMenu::SelectEngine()
  //{
  //  // Get all the compositions on the engine
  //  YTE::Composition *engine = static_cast<YTELevelEditor*>(mWorkspace)->GetRunningEngine();
  //
  //  mObjectBrowser->SelectNoItem();
  //
  //  mComponentTree->LoadGameObject(engine);
  //}

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
