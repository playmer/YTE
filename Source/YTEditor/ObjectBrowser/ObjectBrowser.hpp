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

#include <vector>

#include <qtreewidget.h>


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

  class ObjectBrowser : public QTreeWidget
  {
  public:
    ObjectBrowser(MainWindow* aMainWindow, QWidget* aParent = nullptr);
    ~ObjectBrowser();

    void ClearObjectBrowser();

    ObjectItem* AddObject(char const*aCompositionName,
                          char const*aArchetypeName,
                          int aIndex = 0);

    ObjectItem* AddChildObject(char const* aCompositionName,
                               char const* aArchetypeName,
                               ObjectItem* aParentObj,
                               int aIndex = 0);

    ObjectItem* AddTreeItem(char const* aItemName,
                            YTE::Composition* aEngineObj,
                            int aIndex = 0,
                            bool aSetAsCurrent = true);

    ObjectItem* AddTreeItem(char const* aItemName,
                            ObjectItem* aParentObj,
                            YTE::Composition* aEngineObj,
                            int aIndex = 0,
                            bool aSetAsCurrent = true);

    ObjectItem* AddExistingComposition(char const* aCompositionName,
                                       YTE::Composition* aComposition);

    void LoadAllChildObjects(YTE::Composition* aParentObj,
                             ObjectItem* aParentItem);

    YTE::Composition* GetCurrentObject();

    void RemoveObjectFromViewer(ObjectItem* aItem);

    ObjectItem* FindItemByComposition(YTE::Composition* aComp);

    MainWindow* GetMainWindow() const;

    std::vector<ObjectItem*> FindAllObjectsOfArchetype(YTE::String& aArchetypeName);

    void SelectNoItem();
    
    void OnCurrentItemChanged(QTreeWidgetItem* aCurrent,
                              QTreeWidgetItem* aPrevious);

    void OnItemSelectionChanged();

    void DuplicateCurrentlySelected();

    void RemoveCurrentObject();

    void SetInsertSelectionChangedCommand(bool isActive);

    void MoveToFrontOfCamera(YTE::Composition *aObject);

  private:
    MainWindow *mMainWindow;
    void SetWidgetSettings();

    void OnItemTextChanged(QTreeWidgetItem* aItem, int aIndex);

    void dropEvent(QDropEvent* aEvent) override;

    void CreateContextMenu(QPoint const& pos);

    void keyPressEvent(QKeyEvent* aEvent);

    ObjectItem* SearchChildrenByComp(ObjectItem* aItem, YTE::Composition* aComp);

    void FindObjectsByArchetypeInternal(YTE::String &aArchetypeName,
                                        std::vector<ObjectItem*>& aResult,
                                        ObjectItem* aItem);

    std::vector<YTE::GlobalUniqueIdentifier> mSelectedItems;
    bool mInsertSelectionChangedCmd;

  };

}
