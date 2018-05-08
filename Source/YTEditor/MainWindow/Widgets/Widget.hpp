#include <qwidget>
#include <qdockwidget.h>

namespace YTEditor
{
  class MainWindow;

  class Widget : public QDockWidget
  {
  public:

    enum DockArea
    {
      Left = Qt::LeftDockWidgetArea,
      Right = Qt::RightDockWidgetArea,
      Top = Qt::TopDockWidgetArea,
      Bottom = Qt::BottomDockWidgetArea
    };

    Widget(const char* aName, DockArea aInitialDock, MainWindow* aMainWindow);

    void SnapToDefaultDock();

  protected:
    MainWindow* mMainWindow;

  private:
    DockArea mDefaultArea;

  };
}
