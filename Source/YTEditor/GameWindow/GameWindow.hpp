/*!
\file   GameWindow.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The subwindow displaying the running game.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qwidget.h>
#include <qwindow.h>

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTEditor
{

  class MainWindow;

  class SubWindow : public QWindow
  {
  public:
    SubWindow(YTE::Window *aWindow, MainWindow *aMainWindow);

    ~SubWindow();

    void resizeEvent(QResizeEvent *aEvent);
    bool nativeEvent(const QByteArray &aEventType, void *aMessage, long *aResult) override;

    void keyPressEvent(QKeyEvent *aEvent);

    YTE::Window *mWindow;

    MainWindow *mMainWindow;
  };

}
