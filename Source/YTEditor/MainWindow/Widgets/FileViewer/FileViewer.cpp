
#include <qmimedata.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/MainWindow/Widgets/FileViewer/FileViewer.hpp"

namespace YTEditor
{

  FileViewerModel::FileViewerModel(QWidget *aParent) : QFileSystemModel(aParent)
  {
    
  }

  Qt::DropActions FileViewerModel::supportedDragActions() const
  {
    return Qt::CopyAction;
  }

  QStringList FileViewerModel::mimeTypes() const
  {
    // set it so we can only drop text
    QStringList types;
    types << "application/vnd.text.list";
    return types;
  }

  QMimeData* FileViewerModel::mimeData(const QModelIndexList &indexes) const
  {
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for (const QModelIndex &index : indexes)
    {
      if (index.isValid())
      {
        QString text = data(index, Qt::DisplayRole).toString();
        stream << text;

        // encode the parent's data as well
        if (index.parent().isValid())
        {
          QString parentData = data(index.parent(), Qt::DisplayRole).toString();
          stream << parentData;
        }
      }
    }

    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
  }


  bool FileViewerModel::canDropMimeData(const QMimeData *data,
                                        Qt::DropAction action,
                                        int row,
                                        int column,
                                        const QModelIndex &parent) const
  {
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);


    if (!data->hasFormat("application/vnd.text.list"))
    {
      return false;
    }

    if (column > 0)
    {
      return false;
    }

    return true;
  }


  bool FileViewerModel::dropMimeData(const QMimeData *data,
                                     Qt::DropAction action,
                                     int row,
                                     int column,
                                     const QModelIndex &parent)
  {
    
    if (!canDropMimeData(data, action, row, column, parent))
    {
      return false;
    }

    if (action == Qt::IgnoreAction)
    {
      return true;
    }


    int beginRow;

    if (row != -1)
    {
      beginRow = row;
    }
    else if (parent.isValid())
    {
      beginRow = parent.row();
    }
    else
    {
      beginRow = rowCount(QModelIndex());
    }

    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;

    int rows = 0;

    while (!stream.atEnd())
    {
      QString text;

      stream >> text;
      newItems << text;
      ++rows;
    }

    insertRows(beginRow, rows, QModelIndex());

    for (const QString &text : newItems)
    {
      QModelIndex idx = index(beginRow, 0, QModelIndex());

      setData(idx, text);
      beginRow++;
    }

    return true;
  }


  FileViewer::FileViewer(QDockWidget *aParent) : QTreeView(aParent)
  {
    FileViewerModel *fileModel = new FileViewerModel(aParent);
    fileModel->setRootPath(QDir::currentPath() + "/../");
    fileModel->setResolveSymlinks(true);

    std::string gamePath = YTE::Path::GetGamePath().String();
    
    this->setModel(fileModel);
    this->setRootIndex(fileModel->index(gamePath.c_str()));

    this->hideColumn(1);
    this->hideColumn(2);
    this->hideColumn(3);

    setDragEnabled(true);
    setDropIndicatorShown(true);
  }
}