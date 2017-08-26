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
}

void ArchetypeTools::Revert()
{
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

    archWStr = L"../Archetypes/" + archWStr + L".json";

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
