/******************************************************************************/
/*!
\file   ComponentBrowser.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The widget handling all subwidgets related to components 
(ArchetypeTools, ComponentTools, ComponentTree)

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <vector>

#include "YTEditor/Framework/Widget.hpp"

class QScrollArea;
class QVBoxLayout;
class QGridLayout;

namespace YTE
{
  class Composition;
  class Component;
}

namespace YTEditor
{
  class YTELevelEditor;
  class ArchetypeTools;
  class ComponentWidget;
  class ComponentTree;
  class ComponentTools;

  class ComponentBrowser : public Framework::Widget
  {
  public:
    ComponentBrowser(YTELevelEditor* editor);

    ComponentTree* GetComponentTree();

    ArchetypeTools* GetArchetypeTools();

    static std::string GetName();
    Framework::Widget::DockArea GetDefaultDockArea() const override;

  private:

    void SetWindowSettings();
    void ConstructSubWidgets();

    QVBoxLayout* mLayout;

    ArchetypeTools* mArchTools;
    ComponentTools* mCompTools;
    ComponentTree* mComponentTree;

  };
}