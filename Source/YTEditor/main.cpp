/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   8/15/2017
 *
 * \copyright All content 2017 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "QtWidgets/QApplication.h"

#include "QtWidgets/QMainWindow.h"
#include "QtWidgets/QDockWidget.h"
#include "QtWidgets/QTextEdit.h"
#include "QtWidgets/QMenuBar.h"
#include "QtWidgets/QToolBar.h"
#include "QtWidgets/QAction.h"
#include "QtWidgets/QTreeWidget.h"
#include <qstylefactory.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/ComponentSystem.h"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "ComponentBrowser/ComponentBrowser.hpp"
#include "ComponentBrowser/ComponentWidget.hpp"
#include "GameWindow/GameWindow.hpp"
#include "MainWindow/YTEditorComponentFactoryInit.hpp"
#include "MainWindow/YTEditorMainWindow.hpp"
#include "MainWindow/YTEditorScriptBind.hpp"
#include "MenuBar/FileMenu.hpp"
#include "ObjectBrowser/ObjectBrowser.hpp"
#include "OutputConsole/OutputConsole.hpp"


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
  YTE::Engine mainEngine{ "../../../../../Assets/Bin/Config", true };

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

  YTE::String camName{ "Camera" };

  // add the camera object to the new level
  YTE::Composition *camera = newLevel->AddComposition<YTE::Composition>(camName, &mainEngine, camName, newLevel);

  // add the camera component to the camera object
  camera->AddComponent(YTE::Camera::GetStaticType());

  // Construct the main window
  YTEditorMainWindow *mainWindow = new YTEditorMainWindow(&mainEngine, &app);

  mainWindow->SetRunningSpaceName(newLevelName);

  // Change the theme/color palette to dark
  SetDarkTheme(app);

  // Tell the window and all its children to display
  mainWindow->show();


  return app.exec();
}
