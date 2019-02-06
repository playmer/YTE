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
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectTree.hpp"

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

  class ObjectBrowser : public Framework::Widget
  {
  public:
    ObjectBrowser(YTELevelEditor* editor);
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

    YTE::Composition* GetCurrentObject();

    void RemoveObjectFromViewer(ObjectItem *aItem);

    ObjectItem* FindItemByComposition(YTE::Composition *aComp);

    std::vector<ObjectItem*> FindAllObjectsOfArchetype(YTE::String &aArchetypeName);

    void SelectNoItem();

    void DuplicateCurrentlySelected();

    void RemoveCurrentObject();

    void SetInsertSelectionChangedCommand(bool isActive);

    void MoveToFrontOfCamera(YTE::Composition *aObject);

    static std::string GetName();

    Widget::DockArea GetDefaultDockArea() const override;

  private:
    ObjectTree* mTree;

    void SetWidgetSettings();

    void dropEvent(QDropEvent *aEvent) override;

    void keyPressEvent(QKeyEvent *aEvent);

    ObjectItem* SearchChildrenByComp(ObjectItem *aItem, YTE::Composition *aComp);

    void FindObjectsByArchetypeInternal(YTE::String &archetypeName,
                                        std::vector<ObjectItem*>& result,
                                        ObjectItem* item);

    std::vector<YTE::GlobalUniqueIdentifier> mSelectedItems;
    bool mInsertSelectionChangedCmd;

  };

}
