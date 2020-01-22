#include "YTE/StandardLibrary/FileSystem.hpp"

#include <qdir.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/Framework/ForwardDeclarations.hpp"
#include "YTEditor/Framework/MainWindow.hpp"
#include "YTEditor/Framework/ToolBarButton.hpp"

#include "YTEditor/YTELevelEditor/Toolbars/GameToolbar.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

YTEditor::GameToolbar::GameToolbar(Framework::MainWindow * aMainWindow)
  : Framework::ToolBar(aMainWindow)
{
  std::filesystem::path workingDir{ YTE::Path::GetEnginePath().String() };
  std::filesystem::path assetsDir{ workingDir.append("CreativeCommons_Icons/") };

  QString iconPath = (assetsDir.generic_string() + "play.png").c_str();
  mPlay = new Framework::ToolBarButton(this, iconPath, "Play Game");
  mPlay->connect(mPlay, &QPushButton::toggled, this, &GameToolbar::PlayToggled);

  iconPath = (assetsDir.generic_string() + "pause.png").c_str();
  mPause = new Framework::ToolBarButton(this, iconPath, "Pause Game");
  mPause->connect(mPause, &QPushButton::toggled, this, &GameToolbar::PauseToggled);
  mPause->SetResetterMode(false);
  mPause->SetIsUncheckable(true);

  iconPath = (assetsDir.generic_string() + "stop.png").c_str();
  mStop = new Framework::ToolBarButton(this, iconPath, "Stop Game");
  mStop->connect(mStop, &QPushButton::pressed, this, &GameToolbar::StopPressed);
  mStop->setCheckable(false);

  AddButton(mPlay);
  AddButton(mPause);
  AddButton(mStop);
}

void YTEditor::GameToolbar::PlayToggled(bool checked)
{
  if (checked)
  {
    
    mMainWindow->GetWorkspace<YTELevelEditor>()->PlayLevel();
  }
}

void YTEditor::GameToolbar::PauseToggled(bool checked)
{
  mMainWindow->GetWorkspace<YTELevelEditor>()->PauseLevel(checked);
}

void YTEditor::GameToolbar::StopPressed()
{
  mMainWindow->GetWorkspace<YTELevelEditor>()->StopLevel();
}
