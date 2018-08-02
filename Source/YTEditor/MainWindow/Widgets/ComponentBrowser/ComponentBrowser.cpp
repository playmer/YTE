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

  ComponentBrowser::ComponentBrowser(MainWindow *aMainWindow)
    : Widget(aMainWindow)
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

  MainWindow* ComponentBrowser::GetMainWindow()
  {
    return mMainWindow;
  }

  std::string ComponentBrowser::GetName() const
  {
    return "ComponentBrowser";
  }

  Widget::DockArea ComponentBrowser::GetDefaultDockPosition() const
  {
    return DockArea::Right;
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