/******************************************************************************/
/*!
\file   ArchetypeTools.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the ArchetypeTools class that contains widgets for
handling an object's archetype.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "ArchetypeTools.hpp"
#include "ComponentBrowser.hpp"
#include "YTEditorMainWindow.hpp"
#include "ObjectBrowser.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Physics/Transform.h"
#include "YTE/Core/AssetLoader.hpp"
#include "ComponentTree.hpp"
#include "ObjectItem.hpp"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>

#include "YTE/Core/Utilities.hpp"
#include <fstream>


ArchetypeTools::ArchetypeTools(ComponentBrowser * aBrowser)
  : QWidget(aBrowser), mBrowser(aBrowser), mLayout(new QHBoxLayout(this)),
  mIsArchetype(false), mIsDifferent(false), mChanges(0)
{
  mLabel = new QLabel("Archetype", this);
  mLayout->addWidget(mLabel);

  mArchNameBar = new QLineEdit(this);
  mLayout->addWidget(mArchNameBar);

  mRevertButton = new QPushButton("RV", this);
  mRevertButton->setMaximumSize(20, 20);
  mLayout->addWidget(mRevertButton);
  connect(mRevertButton, &QPushButton::clicked, this, &ArchetypeTools::Revert);

  mSaveAsButton = new QPushButton("SA", this);
  mSaveAsButton->setMaximumSize(20, 20);
  mLayout->addWidget(mSaveAsButton);
  connect(mSaveAsButton, &QPushButton::clicked, this, &ArchetypeTools::SaveAs);

  mOverwriteButton = new QPushButton("OW", this);
  mOverwriteButton->setMaximumSize(20, 20);
  mLayout->addWidget(mOverwriteButton);
  connect(mOverwriteButton, &QPushButton::clicked, this, &ArchetypeTools::Overwrite);
}

ArchetypeTools::~ArchetypeTools()
{
}

int ArchetypeTools::IncrementChanges()
{
  mRevertButton->show();
  mOverwriteButton->show();
  mIsDifferent = true;
  return ++mChanges;
}

int ArchetypeTools::DecrementChanges()
{
  if (--mChanges == 0)
  {
    ClearChanges();
  }

  return mChanges;
}

void ArchetypeTools::ClearChanges()
{
  mChanges = 0;
  mIsDifferent = false;
  HideButtons();
}

QLineEdit * ArchetypeTools::GetLineEdit()
{
  return mArchNameBar;
}

void ArchetypeTools::SetButtonMode(int aMode)
{

  mRevertButton->show();
  mSaveAsButton->show();
  mOverwriteButton->show();

  /*
  switch (aMode)
  {
  case NoArchetype:
  {
    mRevertButton->hide();
    mSaveAsButton->show();
    mOverwriteButton->hide();
    break;
  }

  case IsSame:
  {
    mRevertButton->hide();
    mSaveAsButton->hide();
    mOverwriteButton->hide();
    break;
  }

  case HasChanged:
  {
    mRevertButton->show();
    mSaveAsButton->hide();
    mOverwriteButton->show();
    break;
  }
  }
  */
}

void ArchetypeTools::Revert()
{
  // diff RSValues to see if reloading is necessary
  YTEditorMainWindow *mainWin = mBrowser->GetMainWindow();

  YTE::Composition *obj = mainWin->GetObjectBrowser().GetCurrentObject();
  YTE::RSAllocator allocator;
  YTE::RSValue currArchValue = obj->Serialize(allocator);

  YTE::String archName = mArchNameBar->text().toStdString();

  YTE::RSValue *trueArchValue = mainWin->GetRunningEngine()->GetArchetype(archName);

  // if they're different, we need to reinstantiate all the objects
  if (currArchValue != *trueArchValue)
  {
    // save the position and rotation
    YTE::Transform *trans = obj->GetComponent<YTE::Transform>();
    auto pos = trans->GetWorldTranslation();
    auto rot = trans->GetWorldRotation();

    YTE::String name = obj->GetName();

    // delete the old instances
    YTE::Composition *parent = obj->GetParent();
    parent->RemoveComposition(obj);
    
    
    ObjectBrowser &objBrowser = mBrowser->GetMainWindow()->GetObjectBrowser();
    
    ObjectItem *currItem = dynamic_cast<ObjectItem*>(objBrowser.currentItem());

    // is the item parented?
    bool isChild = currItem->parent();
    int index = 0;

    if (isChild)
    {
      index = currItem->parent()->indexOfChild(currItem);
    }
    else
    {
      index = objBrowser.indexOfTopLevelItem(currItem);
    }

    // remove old object item
    objBrowser.RemoveObjectFromViewer(currItem);

    ObjectItem *newItem = nullptr;

    // item has a parent object
    if (isChild)
    {
      newItem = objBrowser.AddChildObject(name.c_str(), 
                                          archName.c_str(), 
                                          objBrowser.FindItemByComposition(parent), 
                                          index);
    }
    // item has no parent
    else
    {
      newItem = objBrowser.AddObject(name.c_str(), archName.c_str(), index);
    }

    // set obj to the new composition
    obj = newItem->GetEngineObject();

    // load all the children of the new object
    //objBrowser.LoadAllChildObjects(obj, newItem);

    // set their positions and rotations to the stored values
    trans = obj->GetComponent<YTE::Transform>();
    trans->SetWorldTranslation(pos);
    trans->SetWorldRotation(rot);
  }

  mIsDifferent = false;
  ClearChanges();
}

void ArchetypeTools::SaveAs()
{
  mIsArchetype = true;
  mIsDifferent = false;

  YTE::Composition *cmp = mBrowser->GetMainWindow()->GetObjectBrowser().GetCurrentObject();

  std::string str = mArchNameBar->text().toStdString();
  YTE::String arch = str.c_str();

  cmp->SetArchetypeName(arch);

  YTE::RSAllocator allocator;

  YTE::RSValue archetype = cmp->Serialize(allocator);

  YTE::RSStringBuffer sb;
  YTE::RSPrettyWriter writer(sb);
  archetype.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
  std::string archOut = sb.GetString();

  std::string archName(arch.c_str());
  std::wstring archWStr{ archName.begin(), archName.end() };

  std::string path = YTE::Path::GetGamePath().String();
  std::wstring pathWStr{ path.begin(), path.end() };

  archWStr = pathWStr + L"Archetypes/" + archWStr + L".json";

  std::ofstream newArch;
  newArch.open(archWStr);
  newArch << archOut;
  newArch.close();

  mBrowser->GetMainWindow()->SaveCurrentLevel();
}

void ArchetypeTools::Overwrite()
{
  mIsDifferent = false;
  ClearChanges();
}

void ArchetypeTools::HideButtons()
{
  mRevertButton->hide();
  mSaveAsButton->hide();
  mOverwriteButton->hide();
}
