#include <qmenubar.h>

#include "YTEditor/Framework/MainWindow.hpp"
#include "YTEditor/Framework/Workspace.hpp"

namespace YTEditor
{
namespace Framework
{

MainWindow::MainWindow()
{
  QMainWindow::setMenuBar(new QMenuBar(this));
}

} // End of Framework namespace
} // End of Editor namespace
