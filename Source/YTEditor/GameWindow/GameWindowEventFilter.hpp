#pragma once

#include <qobject.h>


namespace YTEditor
{
  class MainWindow;

  class GameWindowEventFilter : public QObject
  {
  public:
    GameWindowEventFilter(QObject *aParent, MainWindow *aMainWin);

    bool eventFilter(QObject *aWatched, QEvent *aEvent) override;

  private:
    MainWindow *mMainWindow;

  };
  

}