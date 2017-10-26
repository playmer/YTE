/******************************************************************************/
/*!
\file   GameWindow.cpp
\author Joshua Fisher
\par    email: j.fisher\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the GameWindow that displays the running game.
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include <qevent.h>
#include <qtimer.h>
#include <qwindow.h>


#include "YTE/Platform/TargetDefinitions.hpp"
#include "YTE/Platform/Window.hpp"

#ifdef Windows
#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"
#endif

#include "YTE/Core/Engine.hpp"

#include "YTEditor/GameWindow/GameWindow.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"

namespace YTEditor
{

  SubWindow::SubWindow(YTE::Window *aWindow, MainWindow *aMainWindow)
    : mWindow(aWindow), mMainWindow(aMainWindow)
  {
  }

  void SubWindow::resizeEvent(QResizeEvent *aEvent)
  {
    auto size = aEvent->size();

    YTE::WindowResize event;
    event.height = size.height();
    event.width = size.width();

    mWindow->SendEvent(YTE::Events::WindowResize, &event);
    mWindow->mEngine->Update();
  }

  void SubWindow::keyPressEvent(QKeyEvent * aEvent)
  {
    mMainWindow->keyPressEvent(aEvent);
    aEvent->ignore();
  }

  bool SubWindow::nativeEvent(const QByteArray &aEventType, void *aMessage, long *aResult)
  {
    auto qtVal = QWindow::nativeEvent(aEventType, aMessage, aResult);

    // TODO Implement this on other platforms maybe?
#ifdef Windows
    MSG message = *(static_cast<MSG*>(aMessage));

    TranslateMessage(&message);

    if ((WM_SIZE != message.message) &&
      (WM_DESTROY != message.message) &&
      (WM_CLOSE != message.message) &&
      (WM_NCDESTROY != message.message) &&
      (0x90 /*WM_UAHDESTROYWINDOW*/ != message.message))
    {
      YTE::Window::MessageHandler(message.hwnd,
        message.message,
        message.wParam,
        message.lParam,
        mWindow);
    }
#endif

    return qtVal;
  }

}