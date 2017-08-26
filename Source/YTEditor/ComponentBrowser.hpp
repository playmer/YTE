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
#include <QtWidgets/qwidget.h>

class ComponentWidget;
class ComponentTree;
class ComponentTools;
class QScrollArea;
class QVBoxLayout;
class QGridLayout;
class YTEditorMainWindow;
class ArchetypeTools;

namespace YTE
{
  class Composition;
  class Component;
}

class ComponentBrowser : public QWidget
{
public:

  ComponentBrowser(YTEditorMainWindow * mainWindow, QWidget * parent = nullptr);
  ~ComponentBrowser();

  ComponentTree* GetComponentTree();
  
  ArchetypeTools* GetArchetypeTools();

  YTEditorMainWindow * GetMainWindow();

private:

  void SetWindowSettings();
  void ConstructSubWidgets();

  QVBoxLayout *mLayout;

  ArchetypeTools *mArchTools;
  ComponentTools *mCompTools;
  ComponentTree *mComponentTree;

  YTEditorMainWindow * mMainWindow;

};