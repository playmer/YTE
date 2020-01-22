#pragma once

#include "YTEditor/Framework/ForwardDeclarations.hpp"
#include "YTEditor/Framework/ToolBar.hpp"

namespace YTEditor
{
  class GameToolbar : public Framework::ToolBar
  {
  public:
    GameToolbar(Framework::MainWindow* aMainWindow);

  private:
    
    Framework::ToolBarButton *mPlay;
    void PlayToggled(bool checked);

    Framework::ToolBarButton *mPause;
    void PauseToggled(bool checked);

    Framework::ToolBarButton *mStop;
    void StopPressed();

  };
}