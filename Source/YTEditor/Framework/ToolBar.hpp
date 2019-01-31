#pragma once

#include <memory>

#include <qtoolbar.h>

namespace YTEditor
{
namespace Framework
{

class Workspace;

class ToolBar : public QToolBar
{
public:
  ToolBar(Workspace* workspace);

private:
  friend class Workspace;
  Workspace* mWorkspace;
};

} // End of Framework namespace
} // End of Editor namespace
