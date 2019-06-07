#include "YTE/StandardLibrary/FileSystem.hpp"

#include <qmimedata.h>
#include <QEvent>
#include <QDragEnterEvent>

#include "YTE/Core/Space.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Physics/Orientation.hpp"

#include "YTEditor/YTELevelEditor/MenuBar/GameObjectMenu.hpp"

#include "YTEditor/YTELevelEditor/Widgets/FileViewer/FileViewer.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"
#include "YTEditor/YTELevelEditor/Widgets/GameWindow/GameWindowEventFilter.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{

  GameWindowEventFilter::GameWindowEventFilter(QObject* aParent, 
                                               Framework::MainWindow* aMainWin)
    : QObject(aParent)
    , mMainWindow(aMainWin)
  {
  }

  bool GameWindowEventFilter::eventFilter(QObject* aWatched,
                                          QEvent* aEvent)
  {
    YTE::UnusedArguments(aWatched);

    auto editor = mMainWindow->GetWorkspace<YTELevelEditor>();

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

      namespace fs = std::filesystem;
      fs::path droppedFile{ filename };
      std::string stem = droppedFile.stem().generic_string();
      std::string extension = droppedFile.extension().generic_string();

      OutputConsole *console = editor->GetWidget<OutputConsole>();

      // we can assume this is coming from model or animation folder
      if (extension == ".fbx")
      {
        if (parent == "Models")
        {
          editor->GetGameObjectMenu()->MakeObject(stem.c_str(), filename.c_str());
        }
        else if (parent == "Animations")
        {
          console->PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for animations.");
        }
      }
      else if (extension == ".json")
      {
        if (parent == "Archetypes")
        {
          ObjectBrowser *objBrowser = editor->GetWidget<ObjectBrowser>();

          ObjectItem *objItem = objBrowser->AddObject(stem.c_str(), stem.c_str());
          objBrowser->MoveToFrontOfCamera(objItem->GetEngineObject());

          /*
          YTE::Transform *transform = objItem->GetEngineObject()->GetComponent<YTE::Transform>();

          if (transform)
          {
            auto view = mMainWindow->GetEditingLevel()->GetComponent<YTE::GraphicsView>();
            auto cameraComponent = view->GetActiveCamera();
            auto cameraObject = cameraComponent->GetOwner();
            auto cameraTransform = cameraObject->GetComponent<YTE::Transform>();
            glm::vec3 camPos = cameraTransform->GetWorldTranslation();
            YTE::Orientation *orientation = cameraObject->GetComponent<YTE::Orientation>();
            glm::vec3 forwardVec = orientation->GetForwardVector();

            transform->SetWorldTranslation(camPos + 40.0f * forwardVec);
          }
          */
        }
        else if (parent == "Levels")
        {
          console->PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for levels.");
        }
      }
      else if (parent == "Originals")
      {
        console->PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for textures.");
      }
      else if (parent == "Crunch")
      {
        console->PrintLnC(OutputConsole::Color::Red, "Drag and drop not supported for compressed textures.");
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
        editor->GetWidget<ObjectBrowser>()->RemoveCurrentObject();
      }
    }

    return false;
  }

}


