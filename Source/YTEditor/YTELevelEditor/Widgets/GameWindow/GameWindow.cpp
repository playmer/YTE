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

  void SubWindow::resizeEvent(QResizeEvent* aEvent)
  {
    if (nullptr == mWindow)
    {
      return;
    }

    auto size = aEvent->size();

    YTE::WindowResize event;
    event.height = size.height();
    event.width = size.width();

    mWindow->SetInternalDimensions(event.width, event.height);

    mWindow->SendEvent(YTE::Events::WindowResize, &event);

    // We check this because Qt might send us this resize event as soon as
    // the window is made, which might be before the Engine is initialized.
    if (mWindow->mEngine->IsInitialized())
    {
      mWindow->mEngine->Update();
    }
  }

  void SubWindow::Update()
  {
    if (nullptr == mWindow)
    {
      return;
    }

    auto place = geometry();
    auto topLeft = mapToGlobal(QPoint{ 0,0 });

    mWindow->SetInternalPosition(topLeft.x(), topLeft.y());
  }

  void SubWindow::keyPressEvent(QKeyEvent* aEvent)
  {
    if (nullptr == mWindow)
    {
      return;
    }

    mLevelEditor->GetMainWindow<YTEditorMainWindow>()->keyPressEvent(aEvent);
    aEvent->ignore();
  }

  void SubWindow::focusInEvent(QFocusEvent*)
  {
    mLevelEditor->GetRunningEngine()->GetPlatformManager()->SetKeyboardFocusedWindow(mWindow);
  }

  void SubWindow::focusOutEvent(QFocusEvent*)
  {
    mLevelEditor->GetRunningEngine()->GetPlatformManager()->SetKeyboardFocusedWindow(nullptr);
  }

}