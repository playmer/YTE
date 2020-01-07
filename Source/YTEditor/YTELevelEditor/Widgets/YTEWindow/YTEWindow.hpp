#pragma once

#include <QWidget>
#include <QWindow>

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

namespace YTEditor
{
  class YTEWindow : public QWindow
  {
  public:
    YTEWindow(YTE::Window *aWindow, YTELevelEditor* aLevelEditor);

    ~YTEWindow();

    void Update();

    void resizeEvent(QResizeEvent* aEvent) override;
    void keyPressEvent(QKeyEvent* aEvent) override;
    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;

    YTE::Window* mWindow;
    YTELevelEditor* mLevelEditor;
  };

}
