#include <qmenubar.h>

#include "YTEditor/Framework/MainWindow.hpp"
#include "YTEditor/Framework/Workspace.hpp"

namespace YTEditor
{
namespace Framework
{

  MainWindow::MainWindow()
    : mMenuBar{ new QMenuBar(this) }
  {
    QMainWindow::setMenuBar(mMenuBar);
  }

} // End of Framework namespace
} // End of Editor namespace
