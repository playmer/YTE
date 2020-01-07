#include <QEvent>
#include <QTimer>
#include <QWindow>

#include "YTE/Platform/TargetDefinitions.hpp"
#include "YTE/Platform/Window.hpp"

#include "YTE/Core/Engine.hpp"

#include "YTEditor/YTELevelEditor/Widgets/YTEWindow/YTEWindow.hpp"

#include "YTEditor/YTELevelEditor/YTEditorMainWindow.hpp"
#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{

  YTEWindow::YTEWindow(YTE::Window *aWindow, YTELevelEditor* aLevelEditor)
    : mWindow{ aWindow }
    , mLevelEditor{ aLevelEditor }
  {
  }

  YTEWindow::~YTEWindow()
  {
    mLevelEditor->YTEWindowRemoved(this);
  }

  void YTEWindow::resizeEvent(QResizeEvent* aEvent)
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

  void YTEWindow::Update()
  {
    if (nullptr == mWindow)
    {
      return;
    }

    auto place = geometry();
    auto topLeft = mapToGlobal(QPoint{ 0,0 });

    mWindow->SetInternalPosition(topLeft.x(), topLeft.y());
  }

  void YTEWindow::keyPressEvent(QKeyEvent* aEvent)
  {
    if (nullptr == mWindow)
    {
      return;
    }

    mLevelEditor->GetMainWindow<YTEditorMainWindow>()->keyPressEvent(aEvent);
    aEvent->ignore();
  }

  void YTEWindow::focusInEvent(QFocusEvent*)
  {
    mLevelEditor->GetRunningEngine()->GetPlatformManager()->SetKeyboardFocusedWindow(mWindow);
  }

  void YTEWindow::focusOutEvent(QFocusEvent*)
  {
    mLevelEditor->GetRunningEngine()->GetPlatformManager()->SetKeyboardFocusedWindow(nullptr);
  }
}