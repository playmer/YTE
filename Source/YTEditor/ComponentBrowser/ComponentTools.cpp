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

#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTools.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/ComponentBrowser/ComponentSearchBar.hpp"
#include "YTEditor/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"

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
    MainWindow * mainWindow = mBrowser->GetMainWindow();
    YTE::Engine * engine = mainWindow->GetRunningEngine();
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