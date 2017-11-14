#pragma once

#include "YTEditor/MainWindow/Toolbar.hpp"


namespace YTEditor
{

  class GameToolbar : public Toolbar
  {
  public:
    GameToolbar(MainWindow *aMainWindow);

  private:
    
    ToolbarButton *mPlay;
    void PlayToggled(bool checked);

    ToolbarButton *mPause;
    void PauseToggled(bool checked);

    ToolbarButton *mStop;
    void StopPressed();

  };

}