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

#ifdef YTE_Windows
#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"
#endif

#include "YTE/Core/Engine.hpp"

#include "YTEditor/YTELevelEditor/Widgets/GameWindow/GameWindow.hpp"

#include "YTEditor/YTELevelEditor/YTEditorMainWindow.hpp"
#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{

  SubWindow::SubWindow(YTE::Window *aWindow, YTELevelEditor* aLevelEditor)
    : mWindow{ aWindow }
    , mLevelEditor{ aLevelEditor }
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

    mLevelEditor->GetMainWindow<YTEditorMainWindow>()->keyPressEvent(aEvent);
    aEvent->ignore();
  }

  bool SubWindow::nativeEvent(const QByteArray &aEventType, void *aMessage, long *aResult)
  {
    auto qtVal = QWindow::nativeEvent(aEventType, aMessage, aResult);

    if (nullptr == mWindow)
    {
      return qtVal;
    }

    // TODO Implement this on other platforms maybe?
#ifdef YTE_Windows
    MSG message = *(static_cast<MSG*>(aMessage));

    TranslateMessage(&message);

    if ((WM_DESTROY == message.message) ||
        (WM_CLOSE == message.message)   ||
        (WM_NCDESTROY == message.message))
    {
      mWindow = nullptr;
    }

    if ((WM_SIZE != message.message) &&
        (nullptr != mWindow) &&
        (0x90 /*WM_UAHDESTROYWINDOW*/ != message.message))
    {
      // commented out because window was already destroyed on delete
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