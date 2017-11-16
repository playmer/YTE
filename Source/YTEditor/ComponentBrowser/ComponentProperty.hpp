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
    ComponentProperty(const std::string &aName, MainWindow *aMainWindow, ComponentWidget* aParent)
      : PropertyWidget<T>(aName, aMainWindow, aParent),
      mParentComponent(aParent)
    {
      auto& propMap = aParent->GetEngineComponent()->GetType()->GetProperties();
      auto it_prop = propMap.FindFirst(this->GetName());

      YTE::String tip = it_prop->second->Description();

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
        this->connect(dynamic_cast<QCheckBox*>(this->GetWidgets()[0]),
          &QCheckBox::stateChanged,
          this,
          &ComponentProperty::SaveToEngine);
      }
      else if (std::is_same<QStringList, T>())
      {
        this->connect(dynamic_cast<QComboBox*>(this->GetWidgets()[0]),
          static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &ComponentProperty::SaveToEngine);
      }
      else
      {
        for (int i = 0; i < this->GetWidgets().size(); ++i)
        {
          this->connect(dynamic_cast<QLineEdit*>(this->GetWidgets()[i]),
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

  };

  template <class T>
  void ComponentProperty<T>::SaveToEngine()
  {
    YTE::Type *cmpType = mParentComponent->GetEngineComponent()->GetStaticType();

    auto& propMap = mParentComponent->GetEngineComponent()->GetType()->GetProperties();
    auto it_prop = propMap.FindFirst(this->GetName());
    YTE::Function *getter = it_prop->second.get()->GetGetter();
    YTE::Any *oldVal = new YTE::Any(getter->Invoke(mParentComponent->GetEngineComponent()));

    T val = this->GetPropertyValues();
    YTE::Any *modVal = new YTE::Any(val);

    ArchetypeTools *archTools = mParentComponent->GetMainWindow()->GetComponentBrowser().GetArchetypeTools();

    // Add command to main window undo redo
    auto cmd = std::make_unique<ChangePropValCmd>(cmpType,
      oldVal,
      modVal,
      &mParentComponent->GetMainWindow()->GetOutputConsole(),
      archTools);

    mParentComponent->GetMainWindow()->GetUndoRedo()->InsertCommand(std::move(cmd));

    mParentComponent->GetMainWindow()->GetComponentBrowser().GetArchetypeTools()->IncrementChanges();

    BaseSaveToEngine();
  }

  template<class T>
  void ComponentProperty<T>::BaseSaveToEngine()
  {
    auto& propMap = mParentComponent->GetEngineComponent()->GetType()->GetProperties();
    T value = this->GetPropertyValues();
    auto it_prop = propMap.FindFirst(this->GetName());
    YTE::Function *setter = it_prop->second.get()->GetSetter();
    setter->Invoke(mParentComponent->GetEngineComponent(), value);
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

    auto& propMap = mParentComponent->GetEngineComponent()->GetType()->GetProperties();
    auto it_prop = propMap.FindFirst(this->GetName());
    YTE::Function *getter = it_prop->second.get()->GetGetter();
    
    YTE::Any updatedValue = getter->Invoke(mParentComponent->GetEngineComponent());

    this->SetValue(updatedValue.As<T>());
  }
  

  template <>
  void ComponentProperty<QStringList>::ReloadValueFromEngine();
}