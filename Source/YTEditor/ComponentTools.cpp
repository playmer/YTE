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

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/ComponentSystem.h"

#include "ComponentTools.hpp"
#include "ComponentSearchBar.hpp"
#include "ComponentBrowser.hpp"
#include "ComponentTree.hpp"
#include "ComponentWidget.hpp"
#include "YTEditorMainWindow.hpp"
#include "ObjectBrowser.hpp"

#include <qpushbutton.h>
#include <qlabel.h>

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
  ComponentSearchBar * compSearch = new ComponentSearchBar(this, this);

  // Get the component types
  YTEditorMainWindow * mainWindow = mBrowser->GetMainWindow();
  YTE::Engine * engine = mainWindow->GetRunningEngine();
  YTE::ComponentSystem * system = engine->GetComponent<YTE::ComponentSystem>();

  compSearch->SetComponentList(system->GetComponentTypes());

  mLayout->addWidget(compSearch);
}

ComponentBrowser & ComponentTools::GetBrowser()
{
  return *mBrowser;
}

ComponentSearchBar& ComponentTools::GetSearchBar()
{
  return *mSearchBar;
}