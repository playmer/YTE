#include <qfilesystemmodel.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/FileViewer/FileViewer.hpp"

namespace YTEditor
{
  FileViewer::FileViewer(QDockWidget *aParent) : QTreeView(aParent)
  {
    QFileSystemModel *fileModel = new QFileSystemModel(aParent);
    fileModel->setRootPath(QDir::currentPath() + "/../");
    fileModel->setResolveSymlinks(true);

    std::string gamePath = YTE::Path::GetGamePath().String();
    
    this->setModel(fileModel);
    this->setRootIndex(fileModel->index(gamePath.c_str()));
  }
}