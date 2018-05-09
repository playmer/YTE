#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/Widgets/Widget.hpp"

namespace YTEditor
{
  Widget::Widget(MainWindow* aMainWindow)
    : QWidget()
    , mMainWindow(aMainWindow)
  {
  }
}
