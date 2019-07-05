#pragma once

#include <memory>

#include <QWidget>
#include <QDockWidget>

#include "ToolWindowManager.h"


namespace YTEditor
{
  namespace Framework
  {
    class Workspace;

    class Widget : public QWidget
    {
    public:
      Widget(Workspace* workspace);

      virtual ToolWindowManager::AreaReference GetToolArea() = 0;

      virtual ToolWindowManager::ToolWindowProperty GetToolProperties()
      {
        return ToolWindowManager::ToolWindowProperty(0);
      };

      template <typename T = Workspace>
      T * GetWorkspace() const
      {
        return static_cast<T*>(mWorkspace);
      }

    protected:
      std::unique_ptr<QDockWidget> mDockWidget;
      Workspace* mWorkspace;

    private:
      friend class Workspace;
    };
  } // End of Framework namespace
} // End of Editor namespace
