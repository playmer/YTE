#include "YTEditor/Framework/ToolBarButton.hpp"
#include "YTEditor/Framework/ToolBar.hpp"

namespace YTEditor
{
  namespace Framework
  {

    ToolBar::ToolBar(MainWindow* aMainWindow)
      : QToolBar()
      , mMainWindow(aMainWindow)
    {

    }

    void ToolBar::AddButton(ToolBarButton* aButton)
    {
      this->addWidget(aButton);
      mButtons.emplace_back(aButton);
    }

  } // End of Framework namespace
} // End of Editor namespace
