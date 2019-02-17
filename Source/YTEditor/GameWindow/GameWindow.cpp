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

#if YTE_Windows
#include "YTE/Platform/Win32/WindowsInclude_Win32.hpp"
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

  SubWindow::~SubWindow()
  {
  }

  void SubWindow::resizeEvent(QResizeEvent *aEvent)
  {
    if (nullptr == mWindow)
    {
      return;
    }

    auto size = aEvent->size();

    YTE::WindowResize event;
    event.height = size.height();
    event.width = size.width();

    mWindow->SetHeight(event.height);
    mWindow->SetWidth(event.width);

    mWindow->SendEvent(YTE::Events::WindowResize, &event);

    // We check this because Qt might send us this resize event as soon as
    // the window is made, which might be before the Engine is initialized.
    if (mWindow->mEngine->IsInitialized())
    {
      mWindow->mEngine->Update();
    }
  }

  void SubWindow::keyPressEvent(QKeyEvent * aEvent)
  {
    if (nullptr == mWindow)
    {
      return;
    }

    mMainWindow->keyPressEvent(aEvent);
    aEvent->ignore();
  }

  void SubWindow::focusInEvent(QFocusEvent*)
  {
    mMainWindow->GetRunningEngine()->GetPlatformManager()->SetKeyboardFocusedWindow(mWindow);
  }

  void SubWindow::focusOutEvent(QFocusEvent*)
  {
    mMainWindow->GetRunningEngine()->GetPlatformManager()->SetKeyboardFocusedWindow(nullptr);
  }
}