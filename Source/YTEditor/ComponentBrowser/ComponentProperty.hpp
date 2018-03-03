/******************************************************************************/
/*!
\file   ComponentProperty.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The class storing and displaying a property on a ComponentWidget.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qcheckbox.h>
#include <qlineedit.h>

#include "YTE/Core/Component.hpp"
#include "YTE/Meta/Type.hpp"

#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/ComponentBrowser/ComponentProperty.hpp"
#include "YTEditor/ComponentBrowser/PropertyWidget.hpp"
#include "YTEditor/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"


namespace YTEditor
{

  class MainWindow;

  template <class T>
  class ComponentProperty : public PropertyWidget<T>
  {
  public:
    ComponentProperty(std::pair<const std::string, std::unique_ptr<YTE::Property>> &aProp, MainWindow *aMainWindow, ComponentWidget* aParent)
      : PropertyWidget<T>(aProp.first, aMainWindow, aParent)
      , mParentComponent(aParent)
      , mEngineProperty(aProp.second.get())
    {
      YTE::String tip = aProp.second->Description();

      mGetter = aProp.second->GetGetter();
      mSetter = aProp.second->GetSetter();

      this->GetLabelWidget()->setToolTip(tip.c_str());

      auto widgets = this->GetWidgets();

      for (auto it : widgets)
      {
        it->setToolTip(tip.c_str());
      }
    }

    void SetEvents()
    {
      if (std::is_same<bool, T>())
      {
        this->connect(static_cast<QCheckBox*>(this->GetWidgets()[0]),
          &QCheckBox::stateChanged,
          this,
          &ComponentProperty::SaveToEngine);
      }
      else if (std::is_same<QStringList, T>())
      {
        QComboBox *combo = static_cast<QComboBox*>(this->GetWidgets()[0]);

        combo->setStyleSheet("combobox-popup: 0;");
        combo->setMaxVisibleItems(25);

        this->connect(combo,
                      static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                      this, &ComponentProperty::SaveToEngine);

      }
      else
      {
        for (int i = 0; i < this->GetWidgets().size(); ++i)
        {
          this->connect(static_cast<QLineEdit*>(this->GetWidgets()[i]),
            &QLineEdit::editingFinished,
            this,
            &ComponentProperty::SaveToEngine);
        }
      }
    }

    ~ComponentProperty() { }

    void SaveToEngine();

    void BaseSaveToEngine();

    void ReloadValueFromEngine();

  private:

    ComponentWidget* mParentComponent;

    YTE::Function *mGetter;
    YTE::Function *mSetter;

    YTE::Property *mEngineProperty;
  };

  template <class T>
  void ComponentProperty<T>::SaveToEngine()
  {
    YTE::Component *cmp = mParentComponent->GetEngineComponent();

    YTE::Any oldVal = mGetter->Invoke(mParentComponent->GetEngineComponent());

    T val = this->GetPropertyValues();
    YTE::Any modVal = YTE::Any(val);

    MainWindow *mainWindow = mParentComponent->GetMainWindow();
    ArchetypeTools *archTools = mainWindow->GetComponentBrowser().GetArchetypeTools();

    std::string name = mEngineProperty->GetName();

    // Add command to main window undo redo
    auto cmd = std::make_unique<ChangePropValCmd>(name,
                                                  cmp->GetGUID(),
                                                  oldVal,
                                                  modVal,
                                                  mainWindow);

    mParentComponent->GetMainWindow()->GetUndoRedo()->InsertCommand(std::move(cmd));

    archTools->IncrementChanges();

    BaseSaveToEngine();
  }

  template<class T>
  void ComponentProperty<T>::BaseSaveToEngine()
  {
    T value = this->GetPropertyValues();
    mSetter->Invoke(mParentComponent->GetEngineComponent(), value);
  }


  template <>
  void ComponentProperty<QStringList>::BaseSaveToEngine();


  template <>
  void ComponentProperty<QStringList>::SaveToEngine();


  template <class T>
  void ComponentProperty<T>::ReloadValueFromEngine()
  {
    std::vector<QWidget*> widgs = this->GetWidgets();

    for (QWidget *w : widgs)
    {
      if (w->hasFocus())
      {
        return;
      }
    }

    YTE::Any updatedValue = mGetter->Invoke(mParentComponent->GetEngineComponent());

    this->SetValue(updatedValue.As<T>());
  }
  

  template <>
  void ComponentProperty<QStringList>::ReloadValueFromEngine();
}