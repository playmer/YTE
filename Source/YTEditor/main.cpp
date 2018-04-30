/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   8/15/2017
 *
 * \copyright All content 2017 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/

#include <qapplication.h>
#include <qmainwindow.h>
#include <qdockwidget.h>
#include <qtextedit.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qtreewidget.h>
#include <qstylefactory.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/FlybyCamera.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Utilities/Utilities.hpp"

#include "YTEditor/MainWindow/ComponentFactoryInit.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/MenuBar/FileMenu.hpp"
#include "YTEditor/MainWindow/ScriptBind.hpp"
#include "YTEditor/MainWindow/SubWindows/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/MainWindow/SubWindows/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/MainWindow/SubWindows/GameWindow/GameWindow.hpp"
#include "YTEditor/MainWindow/SubWindows/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/MainWindow/SubWindows/OutputConsole/OutputConsole.hpp"


// Helper function
void SetDarkTheme(QApplication& app)
{
  app.setStyle(QStyleFactory::create("Fusion"));

  QPalette darkPalette;
  darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::WindowText, Qt::white);
  darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
  darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
  darkPalette.setColor(QPalette::ToolTipText, Qt::white);
  darkPalette.setColor(QPalette::Text, Qt::white);
  darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ButtonText, Qt::white);
  darkPalette.setColor(QPalette::BrightText, Qt::red);
  darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

  darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
  darkPalette.setColor(QPalette::HighlightedText, Qt::black);

  app.setPalette(darkPalette);
  app.setStyleSheet("QToolTip { color: #101010; background-color: #2a82da; border: 1px solid white; }");
}

int main(int argc, char *argv[])
{
  QCoreApplication::addLibraryPath("./");
  QCoreApplication::addLibraryPath("C:/Qt/5.9/msvc2017_64/bin");
  QCoreApplication::addLibraryPath("C:/Qt/5.9/msvc2017_64/plugins");

  // Construct the application
  QApplication app(argc, argv);

  // RUNNING THE GAME
  YTE::InitializeYTETypes();

  YTE::Engine mainEngine{
    {
      "../../../../../Assets/Bin/Config",
      "./Config"
    },
    true
  };

  // initialize types first
  YTEditor::InitializeYTEditorTypes();

  // then create factories for them
  YTE::ComponentSystem *componentSystem = mainEngine.GetComponent<YTE::ComponentSystem>();
  YTE::FactoryMap *factoryMap = componentSystem->GetComponentFactories();
  YTEditor::ComponentFactoryInitialization(&mainEngine, *factoryMap);

  // create an empty level
  YTE::String newLevelName{ "NewLevel" };
  
  // add an empty composition to represent the new level
  YTE::Space *newLevel = mainEngine.AddComposition<YTE::Space>(newLevelName, &mainEngine, nullptr);
  //newLevel->Initialize();
  newLevel->SetPaused(true);
  newLevel->AddComponent(YTE::PhysicsSystem::GetStaticType());


  // load the preferences file
  std::string path = YTE::Path::GetGamePath().String();
  path += "Preferences.json";

  std::unique_ptr<YTE::RSDocument> prefsValue = nullptr;

  std::string buffer;
  bool success = YTE::ReadFileToString(path, buffer);

  if (success)
  {
    prefsValue = std::make_unique<YTE::RSDocument>();

    bool error = prefsValue->Parse(buffer.c_str()).HasParseError();

    if (error)
    {
      prefsValue.release();
      prefsValue = nullptr;
    }
  }

  // Construct the main window
  YTEditor::MainWindow *mainWindow = new YTEditor::MainWindow(&mainEngine, &app, std::move(prefsValue));

  //mainWindow->SetRunningSpaceName();

  // Change the theme/color palette to dark
  SetDarkTheme(app);

  // Tell the window and all its children to display
  mainWindow->show();

  return app.exec();
}
