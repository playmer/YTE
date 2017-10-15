/******************************************************************************/
/*!
\file   ComponentSearchBar.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the search bar for adding components.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Composition.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Graphics/InstantiatedMesh.hpp"
#include "YTE/Graphics/Mesh.hpp"

#include "../MainWindow/YTEditorMainWindow.hpp"
#include "../MaterialViewer/MaterialViewer.hpp"
#include "../OutputConsole/OutputConsole.hpp"
#include "../ObjectBrowser/ObjectBrowser.hpp"
#include "ComponentSearchBar.hpp"
#include "ComponentTools.hpp"
#include "ComponentBrowser.hpp"
#include "ComponentTree.hpp"
#include "SearchBarEventFilter.hpp"


#include <qcompleter.h>

ComponentSearchBar::ComponentSearchBar(ComponentTools *compTools, QWidget *parent) 
  : QLineEdit(parent), mComponentTools(compTools), mCompleter(nullptr)
{
  // signal from qlineedit
  //connect(this, &QLineEdit::returnPressed, this, &ComponentSearchBar::OnReturnPressed);
}

ComponentSearchBar::~ComponentSearchBar()
{
}

void ComponentSearchBar::SetComponentList(const std::vector<YTE::BoundType*>& aTypeList)
{
  if (mCompleter)
  {
    // delete the old completer
    delete mCompleter;
  }

  QStringList compNames;

  for (auto comp : aTypeList)
  {
    compNames.push_back(QString(comp->GetName().c_str()));
  }

  compNames.sort(Qt::CaseInsensitive);

  // create new completer with given list
  mCompleter = new QCompleter(compNames, this);
  mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
  mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
  mCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
  //mCompleter->setCompletionMode(QCompleter::PopupCompletion);

  SearchBarEventFilter * eventFilter = new SearchBarEventFilter(this, mCompleter);

  mCompleter->popup()->installEventFilter(eventFilter);

  this->setCompleter(mCompleter);

  // signal from qcompleter
  connect(mCompleter, static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated), 
    [=](const QString &text) { AddComponent(text); });

  // copy the vector of types
  mComponentTypes = aTypeList;
}

void ComponentSearchBar::OnReturnPressed()
{
  AddComponent(mCompleter->currentCompletion());
}

void ComponentSearchBar::AddComponent(QString aCompName)
{
  std::string stdName = aCompName.toStdString();
  
  YTE::Composition* currObj = this->mComponentTools->GetBrowser().GetMainWindow()->GetObjectBrowser().GetCurrentObject();

  if (nullptr == currObj)
  {
    return;
  }

  YTE::BoundType* type = FindBoundType(stdName);

  auto error = currObj->CheckDependencies(type);

  if (false == error.empty())
  {
    mComponentTools->GetBrowser().GetMainWindow()->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                                                               error.c_str());
    return;
  }

  currObj->AddComponent(type);

  this->mComponentTools->GetBrowser().GetComponentTree()->ClearComponents();
  this->mComponentTools->GetBrowser().GetComponentTree()->LoadGameObject(currObj);

  if (type == YTE::Model::GetStaticType())
  {
    YTEditorMainWindow * mainWindow = mComponentTools->GetBrowser().GetMainWindow();
    YTE::Model * model = mainWindow->GetObjectBrowser().GetCurrentObject()->GetComponent<YTE::Model>();

    if (model && model->GetInstantiatedMesh())
    {
      mainWindow->GetMaterialViewer().LoadMaterial(model->GetInstantiatedMesh()->mMesh->mParts[0].mUBOMaterial);

      // get the list of materials from the submeshes
      auto& submeshes = model->GetInstantiatedMesh()->mMesh->mParts;

      mainWindow->GetMaterialViewer().SetMaterialsList(&submeshes);
    }
  }
}

YTE::Type* ComponentSearchBar::FindBoundType(std::string &aName)
{
  for (auto cType : mComponentTypes)
  {
    if (cType->GetName() == aName)
    {
      return cType;
    }
  }

  return nullptr;
}