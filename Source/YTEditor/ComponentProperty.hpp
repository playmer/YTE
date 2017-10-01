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

#include "PropertyWidget.hpp"

#include "YTE/Core/Component.hpp"
#include "YTE/Meta/Type.hpp"

#include "Commands.hpp"
#include "ArchetypeTools.hpp"

#include <qcheckbox.h>
#include <qlineedit.h>

class YTEditorMainWindow;

template <class T>
class ComponentProperty : public PropertyWidget<T>
{
public:
  ComponentProperty(const std::string &aName, YTEditorMainWindow *aMainWindow, ComponentWidget* aParent)
    : PropertyWidget<T>(aName, aMainWindow, aParent),
      mParentComponent(aParent)
  {
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

private:

  ComponentWidget* mParentComponent;

};

template <class T>
void ComponentProperty<T>::SaveToEngine()
{
  YTE::Type *cmpType = mParentComponent->GetEngineComponent()->GetStaticType();

  auto& propMap = mParentComponent->GetEngineComponent()->GetType()->GetProperties();
  auto it_prop = propMap.FindFirst(this->GetName());
  YTE::Function * getter = it_prop->second.get()->GetGetter();
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
inline void ComponentProperty<T>::BaseSaveToEngine()
{
  auto& propMap = mParentComponent->GetEngineComponent()->GetType()->GetProperties();
  T value = this->GetPropertyValues();
  auto it_prop = propMap.FindFirst(this->GetName());
  YTE::Function * setter = it_prop->second.get()->GetSetter();
  setter->Invoke(mParentComponent->GetEngineComponent(), value);
}


template <>
void ComponentProperty<QStringList>::BaseSaveToEngine()
{
  YTE::Component * comp = this->mParentComponent->GetEngineComponent();

  YTE::Type * type = comp->GetType();

  YTE::OrderedMultiMap<std::string, std::unique_ptr<YTE::Property>>& propMap = type->GetProperties();

  auto it_prop = propMap.FindFirst(this->GetName());

  QStringList value = this->GetPropertyValues();
  YTE::Function * setter = it_prop->second.get()->GetSetter();
  setter->Invoke(mParentComponent->GetEngineComponent(), value[0].toStdString());
}


template <>
void ComponentProperty<QStringList>::SaveToEngine()
{
  // Add command to main window undo redo
  BaseSaveToEngine();
}