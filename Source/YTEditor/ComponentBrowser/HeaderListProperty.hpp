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
#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"


namespace YTEditor
{

  class MainWindow;
  class HeaderListWidget;

  template <class T>
  class HeaderListProperty : public PropertyWidget<T>
  {
  public:
    HeaderListProperty(const std::string &aName, 
                       MainWindow *aMainWindow, 
                       HeaderListWidget *aParent, 
                       YTE::Property *aProp,
                       YTE::Object *aObject)
      : PropertyWidget<T>(aName, aProp, aMainWindow, aParent)
      , mHeaderWidget(aParent)
      , mEngineProp(aProp)
      , mObject(aObject)
    {
      YTE::String tip = aProp->Description();

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
          &HeaderListProperty<T>::SaveToEngine);
      }
      else if (std::is_same<QStringList, T>())
      {
        this->connect(dynamic_cast<QComboBox*>(this->GetWidgets()[0]),
          static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &HeaderListProperty<T>::SaveToEngine);
      }
      else
      {
        for (int i = 0; i < this->GetWidgets().size(); ++i)
        {
          this->connect(dynamic_cast<QLineEdit*>(this->GetWidgets()[i]),
            &QLineEdit::editingFinished,
            this,
            &HeaderListProperty<T>::SaveToEngine);
        }
      }
    }

    ~HeaderListProperty() { }

    void SaveToEngine();

    void BaseSaveToEngine();

    void ReloadValueFromEngine();

  private:

    HeaderListWidget* mHeaderWidget;

    YTE::Property *mEngineProp;

    YTE::Object *mObject;

  };

  template <class T>
  void HeaderListProperty<T>::SaveToEngine()
  {
    //YTE::Function *getter = mEngineProp->GetGetter();
    //YTE::Any *oldVal = new YTE::Any(getter->Invoke(mComponent));
    //
    //T val = this->GetPropertyValues();
    //YTE::Any *modVal = new YTE::Any(val);
    BaseSaveToEngine();
  }

  template<class T>
  void HeaderListProperty<T>::BaseSaveToEngine()
  {
    T value = this->GetPropertyValues();
    YTE::Function *setter = mEngineProp->GetSetter();
    setter->Invoke(mObject, value);
  }


  template <>
  void HeaderListProperty<QStringList>::BaseSaveToEngine();


  template <>
  void HeaderListProperty<QStringList>::SaveToEngine();


  template <class T>
  void HeaderListProperty<T>::ReloadValueFromEngine()
  {
    std::vector<QWidget*> widgs = this->GetWidgets();

    for (QWidget *w : widgs)
    {
      if (w->hasFocus())
      {
        return;
      }
    }

    YTE::Function *getter = mEngineProp->GetGetter();

    YTE::Any updatedValue = getter->Invoke(mObject);

    this->SetValue(updatedValue.As<T>());
  }


  template <>
  void HeaderListProperty<QStringList>::ReloadValueFromEngine();
}