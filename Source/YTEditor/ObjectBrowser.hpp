/******************************************************************************/
/*!
\file   ObjectBrowser.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The tree widget containing and displaying all objects in the level.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <QtWidgets/qtreewidget.h>

// old typedefs from sandbox project
typedef QPair<QString, float> Property;
typedef QVector<Property> Component;
typedef QMap<QString, Component> Archetype;

namespace YTE
{
  class Composition;
}

class YTEditorMainWindow;
class ObjectItem;

class ObjectBrowser : public QTreeWidget
{
public:
  ObjectBrowser(YTEditorMainWindow *aMainWindow, QWidget *parent = nullptr);
  ~ObjectBrowser();

  void ClearObjectBrowser();

  ObjectItem* AddObject(const char *aCompositionName, 
                        const char *aArchetypeName);
  ObjectItem* AddChildObject(const char *aCompositionName,
                             const char *aArchetypeName,
                             ObjectItem *aParentObj);

  ObjectItem* AddTreeItem(const char *aItemName, 
                          YTE::Composition *aEngineObj);
  ObjectItem* AddTreeItem(const char *aItemName, 
                          ObjectItem * aParentObj, 
                          YTE::Composition *aEngineObj);

  ObjectItem* AddExistingComposition(const char *aCompositionName,
                                     YTE::Composition *aComposition);

  void LoadAllChildObjects(YTE::Composition *aParentObj, 
                           ObjectItem *aParentItem);

  YTE::Composition* GetCurrentObject();

  void RemoveObjectFromViewer(ObjectItem *aItem);

  ObjectItem* FindItemByComposition(YTE::Composition *aComp);

  YTEditorMainWindow* GetMainWindow() const;

private:
  YTEditorMainWindow *mMainWindow;
  void SetWidgetSettings();
  void OnCurrentItemChanged(QTreeWidgetItem *current, 
                            QTreeWidgetItem *previous);

  void OnItemTextChanged(QTreeWidgetItem *aItem, int aIndex);

  void CreateContextMenu(const QPoint & pos);

  void RemoveCurrentObject();

  void keyPressEvent(QKeyEvent *aEvent);

  ObjectItem* SearchChildrenByComp(ObjectItem *aItem, YTE::Composition *aComp);
};
