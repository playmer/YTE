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

#include "ComponentBrowser.hpp"
#include "ComponentTree.hpp"
#include "ComponentWidget.hpp"
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qscrollbar.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Meta/Type.hpp"
#include "../MainWindow/YTEditorMainWindow.hpp"
#include "PropertyWidget.hpp"
#include "ComponentTools.hpp"
#include "ArchetypeTools.hpp"

#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Utilities/JsonHelpers.h"

ComponentBrowser::ComponentBrowser(YTEditorMainWindow * mainWindow, QWidget * parent) 
  : QWidget(parent), mMainWindow(mainWindow), mLayout(new QVBoxLayout())
{
  SetWindowSettings();
  this->setLayout(mLayout);
  ConstructSubWidgets();
}

ComponentBrowser::~ComponentBrowser()
{
}

ComponentTree * ComponentBrowser::GetComponentTree()
{
  return mComponentTree;
}

ArchetypeTools * ComponentBrowser::GetArchetypeTools()
{
    return mArchTools;
}

YTEditorMainWindow * ComponentBrowser::GetMainWindow()
{
  return mMainWindow;
}

void ComponentBrowser::SetWindowSettings()
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->setMinimumWidth(300);
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
