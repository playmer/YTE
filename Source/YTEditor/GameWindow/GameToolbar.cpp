#include "YTE/StandardLibrary/FileSystem.hpp"

#include <qdir.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/GameWindow/GameToolbar.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/ToolbarButton.hpp"

YTEditor::GameToolbar::GameToolbar(MainWindow * aMainWindow) : Toolbar(aMainWindow)
{
  std::filesystem::path workingDir{ YTE::Path::GetEnginePath().String() };
  std::filesystem::path assetsDir{ workingDir.append("CreativeCommons_Icons/") };

  QString iconPath = (assetsDir.generic_string() + "play.png").c_str();
  mPlay = new ToolbarButton(this, iconPath);
  mPlay->connect(mPlay, &QPushButton::toggled, this, &GameToolbar::PlayToggled);

  iconPath = (assetsDir.generic_string() + "pause.png").c_str();
  mPause = new ToolbarButton(this, iconPath);
  mPause->connect(mPause, &QPushButton::toggled, this, &GameToolbar::PauseToggled);
  mPause->SetResetterMode(false);
  mPause->SetIsUncheckable(true);

  iconPath = (assetsDir.generic_string() + "stop.png").c_str();
  mStop = new ToolbarButton(this, iconPath);
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
    mMainWindow->PlayLevel();
  }
}

void YTEditor::GameToolbar::PauseToggled(bool checked)
{
  mMainWindow->PauseLevel(checked);
}

void YTEditor::GameToolbar::StopPressed()
{
  mMainWindow->StopLevel();
}
