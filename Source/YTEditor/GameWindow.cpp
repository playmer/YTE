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

#include <QEvent.h>
#include <QTimer.h>
#include <QWindow.h>

#include "YTE/Core/Engine.hpp"



#include "YTE/Platform/TargetDefinitions.hpp"
#include "YTE/Platform/Window.hpp"

#ifdef Windows
#include "YTE/Platform/Windows/WindowsInclude.hpp"
#endif

#include "GameWindow.hpp"

#include "YTEditorMainWindow.hpp"


SubWindow::SubWindow(YTE::Window *aWindow, YTEditorMainWindow *aMainWindow) : 
  mWindow(aWindow), mMainWindow(aMainWindow)
{
}

void SubWindow::resizeEvent(QResizeEvent *aEvent)
{
  auto size = aEvent->size();

  YTE::WindowResizeEvent event;
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
    MSG *message = static_cast<MSG*>(aMessage);
  
    TranslateMessage(message);

    if ((WM_SIZE != message->message)      &&
        (WM_DESTROY != message->message)   &&
        (WM_CLOSE != message->message)     &&
        (WM_KILLFOCUS != message->message) &&
        (WM_ACTIVATE != message->message))
    {
      YTE::Window::MessageHandler(message->hwnd, 
                                  message->message, 
                                  message->wParam, 
                                  message->lParam, 
                                  mWindow);
    }
  #endif

  return qtVal;
}
