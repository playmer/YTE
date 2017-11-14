#include "YTEditor/MainWindow/Toolbar.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/ToolbarButton.hpp"

namespace YTEditor
{

  Toolbar::Toolbar(MainWindow * aMainWindow) : QToolBar(aMainWindow), mMainWindow(aMainWindow)
  {
  }

  std::vector<ToolbarButton*>& Toolbar::GetButtons()
  {
    return mButtons;
  }

  void Toolbar::AddButton(ToolbarButton *aButton)
  {
    addWidget(aButton);
    mButtons.push_back(aButton);
  }
}