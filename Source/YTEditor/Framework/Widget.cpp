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

Widget::DockArea Widget::GetAllowedDockAreas() const
{
  return DockArea::All;
}

} // End of Framework namespace
} // End of Editor namespace
