#pragma once

#include <memory>

#include <qmenu.h>

#include "YTEditor/Framework/ForwardDeclarations.hpp"

namespace YTEditor
{
  namespace Framework
  {

    class Menu : public QMenu
    {
    public:
      Menu(const char* header, Workspace* workspace);

      template<typename T>
      void AddAction(const char* header, void (T::*fn)(), Menu* menu, const char* tooltip = "");

      void AddMenu(Menu* menu);

    protected:
      Workspace* mWorkspace;
    };

    template<typename T>
    void Menu::AddAction(const char* header, void(T::*fn)(), Menu* menu, const char* tooltip)
    {
      QAction* action = new QAction(header);
      addAction(action);
      connect(action, &QAction::triggered, static_cast<T*>(menu), fn);
      action->setToolTip(tooltip);
    }

  } // End of Framework namespace
} // End of Editor namespace
