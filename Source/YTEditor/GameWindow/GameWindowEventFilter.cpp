#include <filesystem>

#include <qmimedata.h>

#include "YTEditor/FileViewer/FileViewer.hpp"
#include "YTEditor/MenuBar/GameObjectMenu.hpp"
#include "YTEditor/GameWindow/GameWindowEventFilter.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"


namespace YTEditor
{

  GameWindowEventFilter::GameWindowEventFilter(QObject *aParent, 
                                               MainWindow *aMainWin)
    : QObject(aParent)
    , mMainWindow(aMainWin)
  {
  }

  bool GameWindowEventFilter::eventFilter(QObject *aWatched, 
                                          QEvent *aEvent)
  {
    QEvent::Type eventType = aEvent->type();

    if (eventType == QEvent::DragMove)
    {
      QDragMoveEvent *moveEvent = static_cast<QDragMoveEvent*>(aEvent);
      moveEvent->acceptProposedAction();

      return true;
    }
    else if (eventType == QEvent::Drop)
    {
      QDropEvent *dropEvent = static_cast<QDropEvent*>(aEvent);

      QObject *sourceObj = dropEvent->source();

      FileViewer *fileViewer = dynamic_cast<FileViewer*>(sourceObj);

      if (!fileViewer)
      {
        dropEvent->ignore();
        return false;
      }

      const QMimeData *rawData = dropEvent->mimeData();

      QByteArray encodedData = rawData->data("application/vnd.text.list");
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

      std::string filename = newItems[0].toStdString();
      std::string parent = newItems[1].toStdString();

      namespace fs = std::experimental::filesystem;
      fs::path droppedFile{ filename };
      std::string extension = droppedFile.extension().generic_string();

      // we can assume this is coming from model or animation folder
      if (extension == ".fbx")
      {
        if (parent == "Models")
        {
          std::cout << "Model dropped : " << filename << std::endl;
          
          mMainWindow->GetGameObjectMenu()->MakeObject(filename.c_str(), filename.c_str());
        }
        else if (parent == "Animations")
        {
          std::cout << "Animation dropped : " << filename << std::endl;
        }
      }
      else if (extension == ".json")
      {
        if (parent == "Archetypes")
        {
          std::cout << "Archetype dropped : " << filename << std::endl;

          std::string trueName = droppedFile.

          mMainWindow->GetObjectBrowser().AddObject(trueName.c_str(), trueName.c_str());
        }
      }

      return true;
    }
    else if (eventType == QEvent::DragEnter)
    {
      QDragEnterEvent *enterEvent = static_cast<QDragEnterEvent*>(aEvent);
      enterEvent->acceptProposedAction();
      return true;
    }
    
    return false;
  }

}


