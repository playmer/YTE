/******************************************************************************/
/*!
\file   ObjectItem.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The tree item class for objects in the ObjectBrowser tree.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <QLabel>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QTreeWidget>


namespace YTE
{
  class Composition;
  class String;
}

namespace YTEditor
{

  class ObjectTree;

  class ObjectItem : public QTreeWidgetItem
  {
  public:
    ObjectItem(YTE::String &aItemName,
      ObjectTree *aParentTree,
      YTE::Composition *aEngineObj,
      YTE::Composition *aEngineLevel);

    ObjectItem(YTE::String &aItemName,
      ObjectItem *aParentItem,
      YTE::Composition *aEngineObj,
      YTE::Composition *aEngineLevel);

    ~ObjectItem();

    void Rename(YTE::String &aName);
    void DeleteFromEngine(YTE::Composition *aParentObj = nullptr);

    ObjectTree *GetObjectTree() const;
    YTE::Composition *GetEngineObject() const;

    void SetEngineObject(YTE::Composition *aComposition);

  private:

    ObjectTree* mObjectTree;
    YTE::Composition* mEngineObject;
    YTE::Composition* mEngineLevel;
  };

  class ObjectItemDelegate : public QStyledItemDelegate
  {
  public:
    ObjectItemDelegate(ObjectTree* aBrowser, QWidget* aParent = nullptr);

    void paint(QPainter *painter,
      const QStyleOptionViewItem &option,
      const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
      QAbstractItemModel *model,
      const QStyleOptionViewItem &option,
      const QModelIndex &index);

  private:
    ObjectTree *mTree;

  };

}
