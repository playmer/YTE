#pragma once

#include <qwidget.h>
#include <qtreeview.h>
#include <qdockwidget.h>
#include <qfilesystemmodel.h>

#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/Framework/ForwardDeclarations.hpp"
#include "YTEditor/Framework/Widget.hpp"

namespace YTEditor
{
  class FileViewerModel : public QFileSystemModel
  {
  public:

    FileViewerModel(QWidget* aParent);

    Qt::DropActions supportedDragActions() const override;

    QStringList mimeTypes() const override;

    QMimeData* mimeData(QModelIndexList const& indexes) const override;

    bool canDropMimeData(QMimeData const* data,
                         Qt::DropAction action,
                         int row,
                         int column,
                         QModelIndex const& parent) const override;

    bool dropMimeData(QMimeData const* data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      QModelIndex const& parent) override;

  };


  class FileViewer : public Framework::Widget
  {
  public:

    FileViewer(Framework::MainWindow* aMainWindow);

    static std::string GetName();
    ToolWindowManager::AreaReference GetToolArea() override;

  private:
    QTreeView mTree;

  };

}
