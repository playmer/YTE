#pragma once

#include <qobject.h>

#include "YTEditor/Framework/ForwardDeclarations.hpp"

namespace YTEditor
{
  class GameWindowEventFilter : public QObject
  {
  public:
    GameWindowEventFilter(QObject *aParent, Framework::MainWindow *aMainWin);

    bool eventFilter(QObject *aWatched, QEvent *aEvent) override;

  private:
    Framework::MainWindow *mMainWindow;

  };
}
