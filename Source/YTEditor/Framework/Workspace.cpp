#include "YTEditor/Framework/Workspace.hpp"

namespace YTEditor
{
namespace Framework
{

Workspace::Workspace(MainWindow* mainWindow)
: mMainWindow(mainWindow)
{
}

const MainWindow* Workspace::GetMainWindow() const
{
  return mMainWindow;
}

} // End of Framework namespace
} // End of YTEditor namespace
