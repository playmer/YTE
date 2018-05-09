#pragma once

#include <qwidget.h>
#include <qtreeview.h>
#include <qdockwidget.h>
#include <qfilesystemmodel.h>

#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/MainWindow/Widgets/Widget.hpp"


namespace YTEditor
{


  class FileViewerModel : public QFileSystemModel
  {
  public:

    FileViewerModel(QWidget *aParent);

    Qt::DropActions supportedDragActions() const override;

    QStringList mimeTypes() const override;

    QMimeData* mimeData(const QModelIndexList &indexes) const override;

    bool canDropMimeData(const QMimeData *data,
                         Qt::DropAction action,
                         int row,
                         int column,
                         const QModelIndex &parent) const override;

    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex &parent) override;

  };


  class FileViewer : public Widget
  {
  public:

    FileViewer(MainWindow *aMainWindow);

  private:
    QTreeView mTree;

  };

}
