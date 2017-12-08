#include <filesystem>

#include <qmimedata.h>

#include "YTEditor/FileViewer/FileViewer.hpp"
#include "YTEditor/MenuBar/GameObjectMenu.hpp"
#include "YTEditor/GameWindow/GameWindowEventFilter.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"


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
      std::string stem = droppedFile.stem().generic_string();
      std::string extension = droppedFile.extension().generic_string();

      OutputConsole &console = mMainWindow->GetOutputConsole();

      // we can assume this is coming from model or animation folder
      if (extension == ".fbx")
      {
        if (parent == "Models")
        {
          mMainWindow->GetGameObjectMenu()->MakeObject(stem.c_str(), filename.c_str());
        }
        else if (parent == "Animations")
        {
          console.PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for animations.");
        }
      }
      else if (extension == ".json")
      {
        if (parent == "Archetypes")
        {
          mMainWindow->GetObjectBrowser().AddObject(stem.c_str(), stem.c_str());
        }
        else if (parent == "Levels")
        {
          console.PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for levels.");
        }
      }
      else if (parent == "Originals")
      {
        console.PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for textures.");
      }
      else if (parent == "Crunch")
      {
        console.PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for compressed textures.");
      }

      return true;
    }
    else if (eventType == QEvent::DragEnter)
    {
      QDragEnterEvent *enterEvent = static_cast<QDragEnterEvent*>(aEvent);
      enterEvent->acceptProposedAction();
      return true;
    }
    else if (eventType == QEvent::KeyPress)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(aEvent);

      if (keyEvent->key() == Qt::Key_Delete)
      {
        mMainWindow->GetObjectBrowser().RemoveCurrentObject();
      }
    }
    
    return false;
  }

}


