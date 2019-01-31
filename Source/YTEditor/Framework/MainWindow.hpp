#pragma once

#include <map>
#include <typeindex>
#include <memory>

#include <qmainwindow.h>

namespace YTEditor
{
namespace Framework
{

class Workspace;

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
  std::map<std::type_index, Framework::Workspace*> mWorkspaces;
};

template <typename T, typename... Args>
T* MainWindow::LoadWorkspace(Args&&... args)
{
  auto inserted = mWorkspaces.emplace(std::type_index(typeid(T)), new T(std::forward<Args>(args)...));

  if (inserted.second)
  {
    if (inserted.first->second->Initialize())
    {
      return static_cast<T*>(inserted.first->second);
    }
  }
  return nullptr;
}

template <typename T>
bool MainWindow::UnloadWorkspace(T* workspace)
{
  auto it = mWorkspaces.find(std::type_index(typeid(T)));

  if (it != mWorkspaces.end())
  {
    it->second->Shutdown();
    mWorkspaces.erase(it);
    return true;
  }
  return false;
}

} // End of Framework namespace
} // End of Editor namespace
