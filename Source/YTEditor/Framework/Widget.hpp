#pragma once

#include <memory>

#include <QWidget>
#include <QDockWidget>

#include "ToolWindowManager.h"

//#include "YTEditor/Framework/Workspace.hpp"


namespace YTEditor
{
  namespace Framework
  {
    class Workspace;

    // tType must derive from QWidget
    template <typename tInheritingType = QWidget>
    class Widget : public tInheritingType
    {
    public:
      Widget(Workspace* aWorkspace)
        : tInheritingType{}
        , mWorkspace{ aWorkspace }
      {

      }

      virtual ToolWindowManager::AreaReference GetToolArea() = 0;

      virtual ToolWindowManager::ToolWindowProperty GetToolProperties()
      {
        return ToolWindowManager::ToolWindowProperty(0);
      };

      template <typename tType = Workspace>
      tType* GetWorkspace() const
      {
        return static_cast<tType*>(mWorkspace);
      }

    protected:
      std::unique_ptr<QDockWidget> mDockWidget;
      Workspace* mWorkspace;

    private:
      friend class Workspace;
    };
  } // End of Framework namespace
} // End of Editor namespace
