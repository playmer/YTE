#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/Widgets/Widget.hpp"

namespace YTEditor
{
  Widget::Widget(const char* aName, DockArea aInitialDock, MainWindow* aMainWindow)
    : QDockWidget(aName)
    , mDefaultArea(aInitialDock)
    , mMainWindow(aMainWindow)
  {
    // allow widget to be docked top, bottom, left, right
    setAllowedAreas(Qt::AllDockWidgetAreas);
  }

  void Widget::SnapToDefaultDock()
  {
    // cast to the underlying qt type (the values are the same)
    Qt::DockWidgetArea dockArea = static_cast<Qt::DockWidgetArea>(mDefaultArea);
    mMainWindow->addDockWidget(dockArea, this);
  }
}
