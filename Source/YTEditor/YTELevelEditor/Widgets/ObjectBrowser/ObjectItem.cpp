/******************************************************************************/
/*!
\file   ObjectItem.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the object item class that
represents item in the ObjectBrowser.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <qapplication.h>
#include <qevent.h>
#include <qhboxLayout>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpushbutton.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/YTELevelEditor/MainWindow.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/Commands.hpp"


namespace YTEditor
{
  ObjectItem::ObjectItem(YTE::String &aItemName,
    ObjectTree *aParentTree,
    YTE::Composition *aEngineObj,
    YTE::Composition *aEngineLevel)
    : QTreeWidgetItem(aParentTree),
    mObjectTree(aParentTree),
    mEngineObject(aEngineObj),
    mEngineLevel(aEngineLevel)
  {
    QSignalBlocker blocker(mObjectTree);
    setText(0, aItemName.c_str());
    setFlags(flags() | Qt::ItemIsEditable);
  }

  ObjectItem::ObjectItem(YTE::String &aItemName,
    ObjectItem *aParentItem,
    YTE::Composition *aEngineObj,
    YTE::Composition *aEngineLevel)
    : QTreeWidgetItem(aParentItem),
    mObjectTree(aParentItem->GetObjectTree()),
    mEngineObject(aEngineObj),
    mEngineLevel(aEngineLevel)
  {
    QSignalBlocker blocker(mObjectTree);
    setText(0, aItemName.c_str());
    setFlags(flags() | Qt::ItemIsEditable);
  }

  ObjectItem::~ObjectItem()
  {

  }

  void ObjectItem::Rename(YTE::String &aName)
  {
    mEngineObject->SetName(aName);
  }

  void ObjectItem::DeleteFromEngine(YTE::Composition *aParentObj)
  {
    if (aParentObj == nullptr)
    {
      mEngineLevel->RemoveComposition(mEngineObject);
    }
    else
    {
      aParentObj->RemoveComposition(mEngineObject);
    }

    mEngineObject = nullptr;
  }

  ObjectTree* ObjectItem::GetObjectTree() const
  {
    return mObjectTree;
  }

  YTE::Composition* ObjectItem::GetEngineObject() const
  {
    return mEngineObject;
  }

  void ObjectItem::SetEngineObject(YTE::Composition * aComposition)
  {
    mEngineObject = aComposition;
  }

  ObjectItemDelegate::ObjectItemDelegate(ObjectTree *aBrowser, QWidget * aParent)
    : QStyledItemDelegate(aParent), mTree(aBrowser)
  {
  }

  void ObjectItemDelegate::paint(QPainter * painter,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
  {
    Q_UNUSED(index);

    // paint the default QTreeWidgetItem
    QStyledItemDelegate::paint(painter, option, index);

    QRect r = option.rect;

    // get dimensions and top left corner of button
    int x = r.right() - 30;
    int y = r.top();
    int w = 30;
    int h = r.height();

    // fill out button style (visual representation only)
    QStyleOptionButton button;
    button.rect = QRect(x, y, w, h);
    button.text = "X";
    button.state = QStyle::State_Enabled;

    // draw the button
    QApplication::style()->drawControl(QStyle::CE_PushButton,
      &button, painter);
  }


  bool ObjectItemDelegate::editorEvent(QEvent *event,
    QAbstractItemModel *model,
    const QStyleOptionViewItem &option,
    const QModelIndex &index)
  {
    Q_UNUSED(index);
    Q_UNUSED(model);

    if (event->type() == QEvent::MouseButtonRelease)
    {
      QMouseEvent * e = (QMouseEvent *)event;
      int clickX = e->x();
      int clickY = e->y();

      QRect r = option.rect;//getting the rect of the cell
      int x, y, w, h;
      x = r.left() + r.width() - 30;//the X coordinate
      y = r.top();//the Y coordinate
      w = 30;//button width
      h = 30;//button height

      if (clickX > x && clickX < x + w)
        if (clickY > y && clickY < y + h)
        {
          ObjectItem *item = dynamic_cast<ObjectItem*>(mTree->itemAt(e->pos()));

          if (!item || !item->GetEngineObject())
          {
            return false;
          }

          YTE::Composition *engineObj = item->GetEngineObject();

          OutputConsole* console = mTree->GetMainWindow()->GetWidget<OutputConsole>();

          auto name = item->text(0).toStdString();
          auto cmd = std::make_unique<RemoveObjectCmd>(engineObj, console, mTree);

          mTree->GetMainWindow()->GetUndoRedo()->InsertCommand(std::move(cmd));

          ObjectItem *parent = dynamic_cast<ObjectItem*>(item->parent());

          if (parent)
          {
            YTE::Composition *parentObj = parent->GetEngineObject();
            item->DeleteFromEngine(parentObj);
          }
          else
          {
            item->DeleteFromEngine();
          }

          mTree->RemoveObjectFromViewer(item);

          return true;
        }
    }

    return false;
  }
}
