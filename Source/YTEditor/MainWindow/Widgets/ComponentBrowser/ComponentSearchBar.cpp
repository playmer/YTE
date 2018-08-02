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

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentSearchBar.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentTools.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/SearchBarEventFilter.hpp"
#include "YTEditor/MainWindow/Widgets/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/MainWindow/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/MainWindow/Widgets/OutputConsole/OutputConsole.hpp"

namespace YTEditor
{

  ComponentSearchBar::ComponentSearchBar(ComponentTools *compTools, QWidget *parent)
    : QLineEdit(parent), mComponentTools(compTools), mCompleter(nullptr)
  {
  }

  ComponentSearchBar::~ComponentSearchBar()
  {
  }

  void ComponentSearchBar::SetComponentList(const std::vector<YTE::BoundType*>& aTypeList)
  {
    // replace the list of types
    mComponentTypes = aTypeList;

    if (mCompleter)
    {
      // delete the old completer, must remake becaues of 
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

    QAbstractItemView *popup = mCompleter->popup();

    SearchBarEventFilter *eventFilter = new SearchBarEventFilter(this, mCompleter);
    popup->installEventFilter(eventFilter);
    setCompleter(mCompleter);

    popup->connect(popup, &QAbstractItemView::clicked, this, &ComponentSearchBar::OnReturnPressed);
  }

  void ComponentSearchBar::OnReturnPressed()
  {
    auto index = mCompleter->popup()->currentIndex();
    auto path = mCompleter->pathFromIndex(index);
    std::string pathStr = path.toStdString();

    clear();
    mCompleter->popup()->hide();

    AddComponent(path);
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

    ObjectBrowser *objBrowser = mainWin->GetWidget<ObjectBrowser>();
    YTE::Composition *currObj = objBrowser->GetCurrentObject();

    if (nullptr == currObj)
    {
      return;
    }

    YTE::BoundType *type = FindBoundType(stdName);

    if (currObj->GetComponent(type) != nullptr)
    {
      return;
    }

    auto error = currObj->HasDependencies(type);

    if (false == error.empty())
    {
      mainWin->GetWidget<OutputConsole>()->PrintLnC(OutputConsole::Color::Red, error.c_str());
      return;
    }

    ComponentTree *compTree = browser.GetComponentTree();
    compTree->AddComponent(type);

    if (type->GetName() == "Model")
    {
      mainWin->GetPhysicsHandler().Remove(currObj);
      mainWin->GetPhysicsHandler().Add(currObj);
    }

    YTE::Model *model = objBrowser->GetCurrentObject()->GetComponent<YTE::Model>();

    if (model && model->GetMesh())
    {
      auto matViewer = mainWin->GetWidget<MaterialViewer>();

      if (matViewer)
      {
        matViewer->LoadMaterial(model->GetMesh()->mParts[0].mUBOMaterial);

        // get the list of materials from the submeshes
        auto& submeshes = model->GetMesh()->mParts;

        matViewer->SetMaterialsList(&submeshes);
      }
    }
  }

  void ComponentSearchBar::ItemActivated(QString aCompName)
  {
    //mCompleter->popup()->hide();
    this->setText("");
    AddComponent(aCompName);
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