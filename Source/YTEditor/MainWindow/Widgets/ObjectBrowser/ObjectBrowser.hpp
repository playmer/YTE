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

#include <qtreewidget.h>

#include "YTE/StandardLibrary/Vector.hpp"

#include "YTEditor/MainWindow/Widgets/Widget.hpp"

// old typedefs from sandbox project
typedef QPair<QString, float> Property;
typedef QVector<Property> Component;
typedef QMap<QString, Component> Archetype;

namespace YTE
{
  class Composition;
  class String;
}

namespace YTEditor
{
  class MainWindow;
  class ObjectItem;

  class ObjectBrowser : public Widget
  {
  public:
    ObjectBrowser(MainWindow *aMainWindow);
    ~ObjectBrowser();

    void ClearObjectList();

    ObjectItem* AddObject(const char *aCompositionName,
                          const char *aArchetypeName,
                          int aIndex = 0);

    ObjectItem* AddChildObject(const char *aCompositionName,
                               const char *aArchetypeName,
                               ObjectItem *aParentObj,
                               int aIndex = 0);

    ObjectItem* AddTreeItem(const char *aItemName,
                            YTE::Composition *aEngineObj,
                            int aIndex = 0,
                            bool aSetAsCurrent = true);

    ObjectItem* AddTreeItem(const char *aItemName,
                            ObjectItem * aParentObj,
                            YTE::Composition *aEngineObj,
                            int aIndex = 0,
                            bool aSetAsCurrent = true);

    ObjectItem* AddExistingComposition(const char *aCompositionName,
                                       YTE::Composition *aComposition);

    void LoadAllChildObjects(YTE::Composition *aParentObj,
                             ObjectItem *aParentItem);

    YTE::Composition* GetCurrentObject();

    void RemoveObjectFromViewer(ObjectItem *aItem);

    ObjectItem* FindItemByComposition(YTE::Composition *aComp);

    MainWindow* GetMainWindow() const;

    YTE::vector<ObjectItem*>* FindAllObjectsOfArchetype(YTE::String &aArchetypeName);

    void SelectNoItem();
    
    void OnCurrentItemChanged(QTreeWidgetItem *current,
                              QTreeWidgetItem *previous);

    void OnItemSelectionChanged();

    void DuplicateCurrentlySelected();

    void RemoveCurrentObject();

    void SetInsertSelectionChangedCommand(bool isActive);

    void MoveToFrontOfCamera(YTE::Composition *aObject);

    std::string GetName() const override;

    Widget::DockArea GetDefaultDockPosition() const override;

  private:
    QTreeWidget mTree;

    void SetWidgetSettings();

    void OnItemTextChanged(QTreeWidgetItem *aItem, int aIndex);

    void dropEvent(QDropEvent *aEvent) override;

    void CreateContextMenu(const QPoint & pos);

    void keyPressEvent(QKeyEvent *aEvent);

    ObjectItem* SearchChildrenByComp(ObjectItem *aItem, YTE::Composition *aComp);

    void FindObjectsByArchetypeInternal(YTE::String &aArchetypeName,
                                        YTE::vector<ObjectItem*>* aResult,
                                        ObjectItem* aItem);

    std::vector<YTE::GlobalUniqueIdentifier> mSelectedItems;
    bool mInsertSelectionChangedCmd;

  };

}
