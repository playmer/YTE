/******************************************************************************/
/*!
\file   ArchetypeTools.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The widget handling all subwidgets for handling an object's archetype.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qwidget.h>
#include <stack>

class ComponentBrowser;
class QPushButton;
class QHBoxLayout;
class QLineEdit;
class QLabel;
class ObjectItem;

class ArchetypeTools : public QWidget
{
public:
  ArchetypeTools(ComponentBrowser *aBrowser);
  ~ArchetypeTools();

  int IncrementChanges();
  int DecrementChanges();
  void ClearChanges();

  QLineEdit* GetLineEdit();

  void SetButtonMode(int aMode);

private:


  ComponentBrowser *mBrowser;
  QHBoxLayout *mLayout;

  QLabel *mLabel;
  QLineEdit *mArchNameBar;

  QPushButton *mRevertButton;
  void Revert();
  void RevertObject(ObjectItem *aObject);

  QPushButton *mSaveAsButton;
  void SaveAs();

  QPushButton *mOverwriteButton;
  void Overwrite();

  bool mIsArchetype; // is the current object based on an archetype
  bool mIsDifferent; // is the current object different than its archetype

  int mChanges;

  void HideButtons();


public:
    enum Mode
    {
        NoArchetype,
        IsSame,
        HasChanged
    };


};