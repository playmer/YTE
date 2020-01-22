/******************************************************************************/
/*!
\file   ComponentBrowser.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the ComponentBrowser that contains the component tools,
archetype tools, and component tree.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <qdockwidget.h>
#include <qlayout.h>
#include <qscrollarea.h>
#include <qscrollbar.h>

#include "YTE/Meta/Meta.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Utilities/JsonHelpers.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTools.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/PropertyWidget.hpp"


namespace YTEditor
{

  ComponentBrowser::ComponentBrowser(YTELevelEditor* editor)
    : Widget(editor)
    , mLayout(new QVBoxLayout())
    , mArchTools(nullptr)
    , mCompTools(nullptr)
    , mComponentTree(nullptr)
  {
    SetWindowSettings();
    setLayout(mLayout);
    ConstructSubWidgets();
  }

  ComponentTree* ComponentBrowser::GetComponentTree()
  {
    return mComponentTree;
  }

  ArchetypeTools* ComponentBrowser::GetArchetypeTools()
  {
    return mArchTools;
  }

  std::string ComponentBrowser::GetName()
  {
    return "ComponentBrowser";
  }

  ToolWindowManager::AreaReference ComponentBrowser::GetToolArea()
  {
    auto workspace = GetWorkspace<YTELevelEditor>();
    auto area = mWorkspace->GetMainWindow()->GetToolWindowManager()->areaOf(workspace->GetLevelWindowWidget());

    return ToolWindowManager::AreaReference{
      ToolWindowManager::AreaReferenceType::RightWindowSide,
      area
    };
  }

  void ComponentBrowser::SetWindowSettings()
  {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setMinimumWidth(300);
    mLayout->setSpacing(1);
  }

  void ComponentBrowser::ConstructSubWidgets()
  {
    mArchTools = new ArchetypeTools(this);
    mLayout->addWidget(mArchTools);
    mArchTools->show();

    mCompTools = new ComponentTools(this);
    mLayout->addWidget(mCompTools);
    mCompTools->show();

    mComponentTree = new ComponentTree(this);
    mLayout->addWidget(mComponentTree);
  }

}