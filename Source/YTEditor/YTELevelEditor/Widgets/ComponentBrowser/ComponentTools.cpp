/******************************************************************************/
/*!
\file   ComponentTools.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the ComponentTools class that contains the search bar
for adding components.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <qpushbutton.h>
#include <qlabel.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/ComponentSystem.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTools.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentSearchBar.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{

  ComponentTools::ComponentTools(ComponentBrowser * parent) : QWidget(parent)
  {
    mBrowser = parent;

    mLayout = new QHBoxLayout(this);
    this->setLayout(mLayout);

    CreateSubWidgets();
  }

  ComponentTools::~ComponentTools()
  {
  }

  void ComponentTools::CreateSubWidgets()
  {
    mLabel = new QLabel("Add Component", this);
    mLayout->addWidget(mLabel);

    // search bar for adding components
    mSearchBar = new ComponentSearchBar(this, this);

    // Get the component types
    YTE::Engine * engine = static_cast<YTELevelEditor*>(mBrowser->GetWorkspace())->GetRunningEngine();
    YTE::ComponentSystem * system = engine->GetComponent<YTE::ComponentSystem>();

    auto compTypes = system->GetComponentTypes();

    mSearchBar->SetComponentList(compTypes);

    mLayout->addWidget(mSearchBar);
  }

  ComponentBrowser & ComponentTools::GetBrowser()
  {
    return *mBrowser;
  }

  ComponentSearchBar& ComponentTools::GetSearchBar()
  {
    return *mSearchBar;
  }
}