#include "YTEditor/Framework/Menu.hpp"

namespace YTEditor
{
  namespace Framework
  {

    Menu::Menu(char const* header, Workspace* workspace)
      : QMenu(header), mWorkspace(workspace)
    {
      setToolTipsVisible(true);
    }

    void Menu::AddMenu(Menu* menu)
    {
      addMenu(menu);
    }

  } // End of Framework namespace
} // End of Editor namespace
