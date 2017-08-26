/******************************************************************************/
/*!
\file   ComponentTools.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The subwidget containing the tools for searching and adding components.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qwidget.h>
#include <qlayout.h>

class ComponentBrowser;
class ComponentSearchBar;
class QLabel;

class ComponentTools : public QWidget
{
public:

  ComponentTools(ComponentBrowser * parent = nullptr);
  ~ComponentTools();

  void CreateSubWidgets();

  ComponentBrowser& GetBrowser();
  ComponentSearchBar& GetSearchBar();

private:

  QHBoxLayout * mLayout;
  ComponentBrowser * mBrowser;

  QLabel *mLabel;
  ComponentSearchBar * mSearchBar;

};