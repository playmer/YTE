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

#include <qcompleter.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentSearchBar.hpp"
#include "YTEditor/ComponentBrowser/ComponentTools.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/ComponentBrowser/SearchBarEventFilter.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"

namespace YTEditor
{

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

  void ComponentSearchBar::OnTabPressed()
  {
    QString tabComplete = mCompleter->currentCompletion();
    mComponentTools->GetSearchBar().setText(tabComplete);
  }

  void ComponentSearchBar::AddComponent(QString aCompName)
  {
    if (aCompName.isEmpty())
    {
      return;
    }

    std::string stdName = aCompName.toStdString();

    ComponentBrowser &browser = mComponentTools->GetBrowser();
    MainWindow *mainWin = browser.GetMainWindow();

    ObjectBrowser &objBrowser = mainWin->GetObjectBrowser();
    YTE::Composition *currObj = objBrowser.GetCurrentObject();

    if (nullptr == currObj)
    {
      return;
    }

    YTE::BoundType *type = FindBoundType(stdName);

    auto error = currObj->CheckDependencies(type);

    if (false == error.empty())
    {
      mainWin->GetOutputConsole().PrintLnC(OutputConsole::Color::Red, error.c_str());
      return;
    }

    currObj->AddComponent(type);

    ComponentTree *compTree = browser.GetComponentTree();
    compTree->ClearComponents();
    compTree->LoadGameObject(currObj);

    if (type->GetName() == "Model")
    {
      mainWin->GetPhysicsHandler().Remove(currObj);
      mainWin->GetPhysicsHandler().Add(currObj);
    }

    YTE::Model *model = objBrowser.GetCurrentObject()->GetComponent<YTE::Model>();

    if (model && model->GetMesh())
    {
      MaterialViewer &matViewer = mainWin->GetMaterialViewer();
      matViewer.LoadMaterial(model->GetMesh()->mParts[0].mUBOMaterial);

      // get the list of materials from the submeshes
      auto& submeshes = model->GetMesh()->mParts;

      matViewer.SetMaterialsList(&submeshes);
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

}