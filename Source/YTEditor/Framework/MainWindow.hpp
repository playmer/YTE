#pragma once

#include <map>
#include <typeindex>
#include <memory>

#include <qmainwindow.h>

#include "YTEditor/Framework/Workspace.hpp"

namespace YTEditor
{
namespace Framework
{

class MainWindow : public QMainWindow
{
public:
  MainWindow();

  /**
  * Creates a workspace of given type
  * Returns pointer to created workspace
  */
  template <typename T, typename... Args>
  T* LoadWorkspace(Args&&... args);

  /**
  * Unloads a workspace
  * Returns true if an instance is unloaded
  * Returns false if no instance of given type is found
  */
  template <typename T>
  bool UnloadWorkspace(T* workspace);

private:
  std::map<std::type_index, std::unique_ptr<Workspace> > mWorkspaces;
};

template <typename T, typename... Args>
T* MainWindow::LoadWorkspace(Args&&... args)
{
  auto inserted = mWorkspaces.emplace({ std::type_index(typeid(T)), std::make_unique<T>(std::forward<Args>(args)...) });
  return inserted->get();
}

template <typename T>
bool MainWindow::UnloadWorkspace(T* workspace)
{
  auto it = mWorkspaces.find(std::type_index(typeid(T)));

  if (it != mWorkspaces.end())
  {
    mWorkspaces.erase(it);
    return true;
  }

  return false;
}

} // End of Framework namespace
} // End of YTEditor namespace
