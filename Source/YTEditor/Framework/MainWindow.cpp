#include "MainWindow.hpp"

#include <qmenubar.h>

namespace YTEditor
{
namespace Framework
{

MainWindow::MainWindow()
{
  setMenuBar(new QMenuBar(this));
}

} // End of Framework namespace
} // End of YTEditor namespace
