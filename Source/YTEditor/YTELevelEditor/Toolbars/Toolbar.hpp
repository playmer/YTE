#pragma once

#include <qtoolbar.h>

namespace YTEditor
{
  class MainWindow;
  class ToolbarButton;

  class Toolbar : public QToolBar
  {
  public:

    Toolbar(MainWindow *aMainWindow);

    std::vector<ToolbarButton*>& GetButtons();

  protected:

    void AddButton(ToolbarButton *aButton);

    MainWindow *mMainWindow;

    std::vector<ToolbarButton*> mButtons;

  };

}