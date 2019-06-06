#include "YTEditor/MainWindow/Toolbars/Toolbar.hpp"
#include "YTEditor/MainWindow/Toolbars/ToolbarButton.hpp"

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