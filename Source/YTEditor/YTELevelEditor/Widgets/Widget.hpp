#pragma once

#include <qwidget>
#include <qdockwidget.h>

namespace YTEditor
{
  class MainWindow;

  class Widget : public QWidget
  {
  public:

    enum DockArea
    {
      Left = Qt::LeftDockWidgetArea,
      Right = Qt::RightDockWidgetArea,
      Top = Qt::TopDockWidgetArea,
      Bottom = Qt::BottomDockWidgetArea
    };

    Widget(MainWindow* aMainWindow);

    virtual std::string GetName() = 0;
    virtual DockArea GetDefaultDockPosition() const = 0;

  protected:
    MainWindow* mMainWindow;
  };
}
