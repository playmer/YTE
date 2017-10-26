#pragma once

#include <qtreeview.h>
#include <qdockwidget.h>

#include "YTE/Utilities/String/String.h"

namespace YTEditor
{

  class FileViewer : public QTreeView
  {
  public:

    FileViewer(QDockWidget *aParent);


  };

}
