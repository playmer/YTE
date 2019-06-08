#pragma once

#include <qtoolbar.h>

#include "YTEditor/Framework/ForwardDeclarations.hpp"

namespace YTEditor
{
  namespace Framework
  {
    class ToolBar : public QToolBar
    {
    public:

      ToolBar(MainWindow* aMainWindow);

      std::vector<ToolBarButton*>& GetButtons()
      {
        return mButtons;
      }

    protected:

      void AddButton(ToolBarButton* aButton);

      MainWindow* mMainWindow;

      std::vector<ToolBarButton*> mButtons;
    };
  }
}