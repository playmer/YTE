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
#include "YTE/Meta/Meta.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentProperty.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/PropertyWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/Commands.hpp"


namespace YTEditor
{
  template <class T>
  class ComponentProperty : public PropertyWidget<T>
  {
  public:
    ComponentProperty(std::pair<const std::string, std::unique_ptr<YTE::Property>>& prop, 
                      YTELevelEditor* editor, 
                      ComponentWidget* parent)
      : PropertyWidget<T>(prop.first, prop.second.get(), editor, parent)
      , mParentComponent(parent)
      , mEngineProperty(prop.second.get())
    {

      mGetter = prop.second->GetGetter();
      mSetter = prop.second->GetSetter();

      // get property tooltip
      std::string const& tip = prop.second->GetDocumentation();

      // set tooltip for property label
      this->GetLabelWidget()->setToolTip(tip.c_str());
      
      auto inputWidgets = this->GetWidgets();

      // set tooltip for all input widgets
      for (auto it : inputWidgets)
      {
        it->setToolTip(tip.c_str());
      }
    }

    void SetEvents()
    {
      // bool so the widget is a checkbox
      if (std::is_same<bool, T>())
      {
        // there is only ever one checkbox widget for a bool
        QCheckBox *checkbox = static_cast<QCheckBox*>(this->GetWidgets()[0]);

        // register to listen for checkbox to change, save to engine when changed
        this->connect(checkbox, &QCheckBox::stateChanged, this, &ComponentProperty::SaveToEngine);
      }
      // list of string, so widget is a dropdown
      else if (std::is_same<QStringList, T>())
      {
        // there is only ever one dropdown widget
        QComboBox *combo = static_cast<QComboBox*>(this->GetWidgets()[0]);

        // adjust combo style and max visible items
        combo->setStyleSheet("combobox-popup: 0;");
        combo->setMaxVisibleItems(25);

        // QComboBox function that we listen to for changes
        auto signalFunc = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);

        // register to listen for index changes, save to engine when changed
        this->connect(combo, signalFunc, this, &ComponentProperty::SaveToEngine);

      }
      // if the property is an editable color
      else if ((std::is_same<glm::vec3, T>() || std::is_same<glm::vec4, T>()) &&
                mEngineProperty->GetAttribute<YTE::EditableColor>())
      {
        // only ever one color picker for a property
        ColorPicker *colPick = static_cast<ColorPicker*>(this->GetWidgets()[0]);

        // save to engine when color picker changes are applied
        this->connect(colPick, &QPushButton::clicked, this, &ComponentProperty::SaveToEngine);
      }
      else
      {
        // unknown number of input widgets, so loop through all
        for (int i = 0; i < this->GetWidgets().size(); ++i)
        {
          QLineEdit *inputWidg = static_cast<QLineEdit*>(this->GetWidgets()[i]);
          
          // save to engine when line edit widget loses focus (user is done making changes)
          this->connect(inputWidg, &QLineEdit::editingFinished, this, &ComponentProperty::SaveToEngine);
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

    // get current value on engine-side
    YTE::Any oldVal = mGetter->Invoke(mParentComponent->GetEngineComponent());

    //get current value editor-side
    T val = this->GetPropertyValues();
    YTE::Any modVal = YTE::Any(val);

    // notify that change has been made to object instance
    MainWindow *mainWindow = mParentComponent->GetMainWindow();
    ArchetypeTools *archTools = mainWindow->GetWidget<ComponentBrowser>()->GetArchetypeTools();
    archTools->IncrementChanges();
  
    std::string name = mEngineProperty->GetName();

    // property value changed to be inserted
    auto cmd = std::make_unique<ChangePropValCmd>(name,
                                                  cmp->GetGUID(),
                                                  oldVal,
                                                  modVal,
                                                  mainWindow);

    // add property changed command to undo/redo
    mainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));

    // actually change the property value engine-side
    BaseSaveToEngine();
  }

  template<class T>
  void ComponentProperty<T>::BaseSaveToEngine()
  {
    // get current property value
    T value = this->GetPropertyValues();

    // call setter to change value engine-side
    mSetter->Invoke(mParentComponent->GetEngineComponent(), value);
  }


  template <>
  void ComponentProperty<QStringList>::BaseSaveToEngine();


  template <>
  void ComponentProperty<QStringList>::SaveToEngine();


  template <class T>
  void ComponentProperty<T>::ReloadValueFromEngine()
  {
    // input widgets for property
    std::vector<QWidget*> inputWidgets = this->GetWidgets();

    for (QWidget *widget : inputWidgets)
    {
      // check if user is changing any of the values
      // if so, return so we don't reload them
      if (widget->hasFocus())
      {
        return;
      }
    }

    // otherwise get the current value engine-side
    YTE::Any updatedValue = mGetter->Invoke(mParentComponent->GetEngineComponent());

    // update value in editor to accurately reflect engine
    this->SetValue(updatedValue.As<T>());
  }
  

  template <>
  void ComponentProperty<QStringList>::ReloadValueFromEngine();
}