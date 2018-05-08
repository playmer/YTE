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

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentTools.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/PropertyWidget.hpp"


namespace YTEditor
{

  ComponentBrowser::ComponentBrowser(MainWindow * mainWindow, QWidget * parent)
    : QWidget(parent), mMainWindow(mainWindow), mLayout(new QVBoxLayout())
  {
    SetWindowSettings();
    this->setLayout(mLayout);
    ConstructSubWidgets();
  }

  ComponentTree * ComponentBrowser::GetComponentTree()
  {
    return mComponentTree;
  }

  ArchetypeTools * ComponentBrowser::GetArchetypeTools()
  {
    return mArchTools;
  }

  MainWindow * ComponentBrowser::GetMainWindow()
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

}