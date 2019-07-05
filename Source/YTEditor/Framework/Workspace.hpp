#pragma once

#include <map>
#include <typeindex>
#include <memory>

#include <qdockwidget.h>
#include <qmenubar.h>

#include "YTEditor/Framework/MainWindow.hpp"
#include "YTEditor/Framework/Menu.hpp"
#include "YTEditor/Framework/ToolBar.hpp"
#include "YTEditor/Framework/Widget.hpp"

namespace YTEditor
{
namespace Framework
{
class MainWindow;

/**
* Base class for defining a toolset of custom widgets
*/
class Workspace
{
public:
  Workspace(MainWindow* mainWindow);

  virtual bool Initialize() = 0;
  virtual void Shutdown() = 0;

  /**
  * Returns first pointer of given type
  */
  template <typename T>
  T* GetWidget() const;

  /**
  * Returns all pointers of given type
  */
  template <typename T>
  std::vector<T*> GetAllWidgets() const;

  /**
  * Returns the MainWindow handling the workspace
  */
  template <typename tType = MainWindow>
  tType* GetMainWindow() const
  {
    return static_cast<tType*>(mMainWindow);
  }

  /**
  * Creates a widget of given type
  * Returns pointer to created widget
  */
  template <typename T, typename... Args>
  T* AddWidget(char const* aName, Args&&... args);

  /**
  * Unloads one instance of a type of widget
  * Returns true if an instance is unloaded, false if no instance of given type is found
  */
  template <typename T>
  bool RemoveWidget(T* widget);

  /**
  * Unload all instances of a type of widget
  * Returns number of instances unloaded
  */
  template <typename T>
  size_t RemoveWidgets();

  /**
  * Creates a toolbar of given type
  * Returns pointer to created toolbar
  */
  template <typename T, typename... Args>
  T* AddToolBar(Args&&... args);

  /**
  * Unloads one instance of a type of toolbar
  * Returns true if an instance is unloaded, false if no instance of given type is found
  */
  template <typename T>
  bool RemoveToolBar(T* widget);

  /**
  * Unload all instances of a type of toolbar
  * Returns number of instances unloaded
  */
  template <typename T>
  size_t RemoveToolBars();

  /**
  * Creates a menu of given type
  * Returns pointer to created menu
  */
  template <typename T, typename... Args>
  T* AddMenu(Args&&... args);

  /**
  * Unloads the menu specified by type
  * Returns true if menu is unloaded, false if menu is not found
  */
  template <typename T>
  bool RemoveMenu(T* menu);

protected:
  friend class MainWindow;
  MainWindow* mMainWindow;

  // Widgets
  std::multimap<std::type_index, std::unique_ptr<QWidget>> mWidgets;

  // Toolbars
  std::multimap<std::type_index, std::unique_ptr<ToolBar>> mToolBars;

  // Menus
  std::map<std::type_index, std::unique_ptr<Menu>> mMenus;
};


// Function Definitions //

template <typename T>
T* Workspace::GetWidget() const
{
  auto it = mWidgets.find(std::type_index(typeid(T)));

  if (it != mWidgets.end())
  {
    return static_cast<T*>(it->second.get());
  }

  return nullptr;
}

template <typename T>
std::vector<T*> Workspace::GetAllWidgets() const
{
  std::vector<T*> result;

  auto range = mWidgets.equal_range(std::type_index(typeid(T)));

  for (auto it = range.first; it != range.second; it++)
  {
    result.emplace_back(it->second.get());
  }

  return result;
}

template <typename T, typename... Args>
T* Workspace::AddWidget(char const* aName, Args&&... args)
{
  // Create Widget
  auto inserted = mWidgets.emplace(std::type_index(typeid(T)), std::make_unique<T>(std::forward<Args>(args)...));

  T* widget = static_cast<T*>(inserted->second.get());

  widget->setWindowTitle(aName);

  mMainWindow->GetToolWindowManager()->addToolWindow(
    widget,
    widget->GetToolArea(), 
    widget->GetToolProperties()
  );

  return widget;
}

template <typename T>
bool Workspace::RemoveWidget(T* widget)
{
  auto range = mWidgets.equal_range(std::type_index(typeid(T)));

  for (auto it = range.first; it != range.second; ++it)
  {
    if (it->get() == widget)
    {
      mWidgets.erase(it);
      return true;
    }
  }

  return false;
}

template <typename T>
size_t Workspace::RemoveWidgets()
{
  return mWidgets.erase(std::type_index(typeid(T)));
}

template <typename T, typename... Args>
T* Workspace::AddToolBar(Args&&... args)
{
  // Create ToolBar
  auto inserted = mToolBars.emplace(std::type_index(typeid(T)), std::make_unique<T>(std::forward<Args>(args)...));

  // Add ToolBar to MainWindow
  mMainWindow->addToolBar(inserted->second.get());
  return static_cast<T*>(inserted->second.get());
}

template <typename T>
bool Workspace::RemoveToolBar(T* toolbar)
{
  auto range = mToolBars.equal_range(std::type_index(typeid(T)));

  for (auto it = range.first; it != range.second; ++it)
  {
    if (it->get() == toolbar)
    {
      mToolBars.erase(it);
      return true;
    }
  }

  return false;
}

template <typename T>
size_t Workspace::RemoveToolBars()
{
  return mToolBars.erase(std::type_index(typeid(T)));
}

template <typename T, typename... Args>
T* Workspace::AddMenu(Args&&... args)
{
  // Create Menu
  auto inserted = mMenus.emplace(std::type_index(typeid(T)), std::make_unique<T>(std::forward<Args>(args)...));

  // Add ToolBar to MainWindow
  QMenuBar* menuBar = mMainWindow->menuBar();
  menuBar->addMenu(inserted.first->second.get());

  return static_cast<T*>(inserted.first->second.get());
}

template <typename T>
bool Workspace::RemoveMenu(T* menu)
{
  return mMenus.erase(std::type_index(typeid(T))) > 0;
}

} // End of Framework namespace
} // End of Editor namespace
