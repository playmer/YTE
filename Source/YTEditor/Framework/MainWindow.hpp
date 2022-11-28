#pragma once

#include <map>
#include <type_traits>
#include <typeindex>
#include <memory>

#include <QMainWindow>

#include "ToolWindowManager.h"

#include "YTEditor/Framework/ForwardDeclarations.hpp"

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
      T* LoadWorkspace(Args&& ... args)
      {
        static_assert(std::is_base_of_v<Workspace, T>, "The type passed to LoadWorkspace must be derived from Workspace.");

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

      /**
      * Unloads a workspace
      * Returns true if an instance is unloaded
      * Returns false if no instance of given type is found
      */
      //template <typename T>
      //bool UnloadWorkspace(T* workspace)
      //{
      //  static_assert(std::is_base_of_v<Workspace, T>, "The type passed to UnloadWorkspace must be derived from Workspace.");
      //
      //  auto it = mWorkspaces.find(std::type_index(typeid(T)));
      //
      //  if (it != mWorkspaces.end())
      //  {
      //    it->second->Shutdown();
      //    mWorkspaces.erase(it);
      //    return true;
      //  }
      //  return false;
      //}

      template <typename T>
      T* GetWorkspace()
      {
        static_assert(std::is_base_of_v<Workspace, T>, "The type passed to GetWorkSpace must be derived from Workspace.");

        auto it = mWorkspaces.find(std::type_index(typeid(T)));

        if (it != mWorkspaces.end())
        {
          return static_cast<T*>(it->second);
        }

        return nullptr;
      }

      ToolWindowManager* GetToolWindowManager()
      {
        return mWindowManager;
      }

    private:
      std::map<std::type_index, Framework::Workspace*> mWorkspaces;
      QMenuBar* mMenuBar;
      ToolWindowManager* mWindowManager;
    };

  } // End of Framework namespace
} // End of Editor namespace
