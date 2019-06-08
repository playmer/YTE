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

#include <fstream>

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/CompositionBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/ObjectItem.hpp"


#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{
  ArchetypeTools::ArchetypeTools(ComponentBrowser * aBrowser)
    : QWidget(aBrowser),
    mBrowser(aBrowser),
    mLayout(new QHBoxLayout(this)),
    mIsArchetype(false),
    mIsDifferent(false),
    mChanges(0)
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
    YTE::UnusedArguments(aMode);

    mRevertButton->show();
    mSaveAsButton->show();
    mOverwriteButton->show();
  }

  void ArchetypeTools::Revert()
  {
    // diff RSValues to see if reloading is necessary
    //MainWindow *mainWin = mBrowser->GetMainWindow();
    CompositionBrowser* objectBrowser = mBrowser->GetWorkspace()->GetWidget<CompositionBrowser>();

    YTE::Composition *obj = objectBrowser->GetCurrentObject();

    RevertObject(obj);

    mIsDifferent = false;
    ClearChanges();
  }

  void ArchetypeTools::RevertObject(ObjectItem * aObject)
  {
    YTE::Composition *obj = aObject->GetEngineObject();

    RevertObject(obj);
  }

  void ArchetypeTools::RevertObject(YTE::Composition * aObject)
  {
    YTE::RSAllocator allocator;
    YTE::RSValue archValue = aObject->Serialize(allocator);

    YTE::String archName = aObject->GetArchetypeName();

    YTE::RSValue *trueArchValue = mBrowser->GetWorkspace<YTELevelEditor>()->GetRunningEngine()->GetArchetype(archName);

    // if they're different, we need to reinstantiate all the objects
    if (archValue != *trueArchValue)
    {
      // save the position and rotation
      YTE::Transform *trans = aObject->GetComponent<YTE::Transform>();
      auto pos = trans->GetTranslation();
      auto rot = trans->GetRotation();

      YTE::String name = aObject->GetName();

      // delete the old instances
      YTE::Composition *parent = aObject->GetParent();
      parent->RemoveComposition(aObject);

      CompositionBrowser* objectBrowser = mBrowser->GetWorkspace()->GetWidget<CompositionBrowser>();

      ObjectItem *item = objectBrowser->FindItemByComposition(aObject);

      // is the item parented?
      bool isChild = item->parent();
      int index = 0;

      if (isChild)
      {
        index = item->parent()->indexOfChild(item);
      }
      else
      {
        index = objectBrowser->indexOfTopLevelItem(item);
      }

      // remove old object item
      objectBrowser->RemoveObjectFromViewer(item);

      ObjectItem *newItem = nullptr;

      // item has a parent object
      if (isChild)
      {
        newItem = objectBrowser->AddChildObject(name.c_str(),
          archName.c_str(),
          objectBrowser->FindItemByComposition(parent),
          index);
      }
      // item has no parent
      else
      {
        newItem = objectBrowser->AddObject(name.c_str(), archName.c_str(), index);
      }

      // set obj to the new composition
      aObject = newItem->GetEngineObject();

      // load all the children of the new object
      objectBrowser->LoadAllChildObjects(aObject, newItem);

      // set their positions and rotations to the stored values
      trans = aObject->GetComponent<YTE::Transform>();
      trans->SetTranslation(pos);
      trans->SetRotation(rot);
    }
  }

  void ArchetypeTools::SaveAs()
  {
    mIsArchetype = true;
    mIsDifferent = false;

    CompositionBrowser* objectBrowser = mBrowser->GetWorkspace()->GetWidget<CompositionBrowser>();

    YTE::Composition *cmp = objectBrowser->GetCurrentObject();

    std::string str = mArchNameBar->text().toStdString();
    YTE::String arch = str.c_str();

    cmp->SetArchetypeName(arch);

    YTE::RSAllocator allocator;

    YTE::RSValue archetype = cmp->Serialize(allocator);

    YTE::RSStringBuffer sb;
    YTE::RSPrettyWriter writer(sb);
    archetype.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    std::string archOut = sb.GetString();

    std::filesystem::path archName{ arch.c_str() };

    std::filesystem::path path{ YTE::Path::GetGamePath().String() };

    archName = archName.concat(".json");

    auto finalPath = path / "Archetypes" / archName;

    std::ofstream newArch;
    newArch.open(finalPath);
    newArch << archOut;
    newArch.close();

    mBrowser->GetWorkspace<YTELevelEditor>()->SaveCurrentLevel();
  }

  void ArchetypeTools::Overwrite()
  {
    std::string str = mArchNameBar->text().toStdString();
    YTE::String arch = str.c_str();

    CompositionBrowser* objectBrowser = mBrowser->GetWorkspace()->GetWidget<CompositionBrowser>();

    auto items = objectBrowser->FindAllObjectsOfArchetype(arch);

    for (auto& obj : items)
    {
      RevertObject(obj);
    }
  }

  void ArchetypeTools::HideButtons()
  {
    mRevertButton->hide();
    mSaveAsButton->hide();
    mOverwriteButton->hide();
  }

}