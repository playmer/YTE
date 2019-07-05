#include "YTEditor/Framework/Widget.hpp"

namespace YTEditor
{
  namespace Framework
  {

    Widget::Widget(Workspace* workspace)
      : QWidget()
    , mWorkspace(workspace)
    {
    }
  } // End of Framework namespace
} // End of Editor namespace
