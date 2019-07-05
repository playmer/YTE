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

#include "YTEditor/Framework/Widget.hpp"

#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/ObjectTree.hpp"

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
  class YTELevelEditor;
  class ObjectItem;

  class CompositionBrowser : public Framework::Widget
  {
  public:
    CompositionBrowser(YTELevelEditor* editor);
    ~CompositionBrowser();

    void ClearObjectList();

    ObjectItem* AddObject(char const* aCompositionName,
                          char const* aArchetypeName,
                          int aIndex = 0);

    ObjectItem* AddChildObject(char const* aCompositionName,
                               char const* aArchetypeName,
                               ObjectItem* aParentObj,
                               int aIndex = 0);

    ObjectItem* AddTreeItem(char const* aItemName,
                            YTE::Composition* aEngineObj,
                            int aIndex,
                            bool aSetAsCurrent);

    ObjectItem* AddTreeItem(char const* aItemName,
                            ObjectItem* aParentObj,
                            YTE::Composition* aEngineObj,
                            int aIndex,
                            bool aSetAsCurrent);

    ObjectItem* AddExistingComposition(char const* aCompositionName,
                                       YTE::Composition* aComposition);


    void LoadAllChildObjects(YTE::Composition* aParentObj, ObjectItem* aParentItem);
    YTE::Composition* GetCurrentObject();
    void setCurrentItem(ObjectItem* aItem);
    void setCurrentItem(ObjectItem* aItem, int aColumn);
    void setItemSelected(ObjectItem* aItem, bool aSelected);
    void clearSelection();
    ObjectItem* topLevelItem(int index);
    void setHeaderLabel(char const* aLabel);
    int indexOfTopLevelItem(ObjectItem* aObject);

    void RemoveObjectFromViewer(ObjectItem* aItem);

    ObjectItem* FindItemByComposition(YTE::Composition* aComp);

    std::vector<ObjectItem*> FindAllObjectsOfArchetype(YTE::String& aArchetypeName);

    void SelectNoItem();

    void DuplicateCurrentlySelected();

    void RemoveCurrentObject();

    void SetInsertSelectionChangedCommand(bool isActive);

    void MoveToFrontOfCamera(YTE::Composition* aObject);

    static std::string GetName();

    ToolWindowManager::AreaReference GetToolArea() override;

  private:
    ObjectTree* mTree;

    void SetWidgetSettings();

    void dropEvent(QDropEvent* aEvent) override;

    void keyPressEvent(QKeyEvent* aEvent);

    ObjectItem* SearchChildrenByComp(ObjectItem* aItem, YTE::Composition* aComp);

    void FindObjectsByArchetypeInternal(YTE::String& archetypeName,
                                        std::vector<ObjectItem*>& result,
                                        ObjectItem* item);

    std::vector<YTE::GlobalUniqueIdentifier> mSelectedItems;
    bool mInsertSelectionChangedCmd;

  };

}
