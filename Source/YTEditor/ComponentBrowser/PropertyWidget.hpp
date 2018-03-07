/******************************************************************************/
/*!
\file   PropertyWidget.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The property widget contained by ComponentWidgets.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <limits>

#include <qcombobox.h>
#include <qformlayout.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qwidget.h>

#include "YTE/Core/Utilities.hpp"

#include "YTEditor/ComponentBrowser/ColorPicker.hpp"
#include "YTEditor/ComponentBrowser/CheckBox.hpp"
#include "YTEditor/ComponentBrowser/LineEdit.hpp"
#include "YTEditor/ComponentBrowser/PropertyWidgetBase.hpp"

namespace YTEditor
{

  template <class T>
  class PropertyWidget : public PropertyWidgetBase
  {
  public:

    PropertyWidget(const std::string &aName, 
                   YTE::Property *aProperty, 
                   MainWindow *aMainWindow, 
                   QWidget * aParent = nullptr)
      : PropertyWidgetBase(aParent)
      , mMainWindow(aMainWindow)
      , mProperty(aProperty)
      , mValues(new QHBoxLayout(this))
      , mPropertyName(aName)
    {
      this->setLayout(mValues);
      mLabel = new QLabel(aName.c_str(), this);
      mValues->addWidget(mLabel);

      // check for the type and add the appropriate widgets
      if constexpr (std::is_same<T, int>())
      {
        mType = PropType::Int;
        AddInteger();
      }
      else if constexpr (std::is_same<T, float>())
      {
        mType = PropType::Float;
        AddFloat();
      }
      else if constexpr (std::is_same<T, YTE::String>())
      {
        mType = PropType::String;
        AddString();
      }
      else if constexpr (std::is_same<T, std::string>())
      {
        mType = PropType::StdString;
        AddStdString();
      }
      else if constexpr (std::is_same<T, bool>())
      {
        mType = PropType::Bool;
        AddBool();
      }
      else if constexpr (std::is_same<T, glm::vec2>())
      {
        mType = PropType::Vec2;
        AddFloat();
        AddFloat();
      }
      else if constexpr (std::is_same<T, glm::vec3>())
      {
        mType = PropType::Vec3;

        if (aProperty->GetAttribute<YTE::EditableColor>())
        {
          auto widg = new ColorPicker(Qt::white, false, this);
          mValues->addWidget(widg);
          mWidgets.push_back(widg);
        }
        else
        {
          AddFloat();
          AddFloat();
          AddFloat();
        }
      }
      else if constexpr (std::is_same<T, glm::vec4>())
      {
        mType = PropType::Vec4;

        if (aProperty->GetAttribute<YTE::EditableColor>())
        {
          auto widg = new ColorPicker(Qt::white, true, this);
          mValues->addWidget(widg);
          mWidgets.push_back(widg);
        }
        else
        {
          AddFloat();
          AddFloat();
          AddFloat();
          AddFloat();
        }
      }
      else if constexpr (std::is_same<T, glm::quat>())
      {
        mType = PropType::Quaternion;
      }
      else if constexpr (std::is_same<T, QStringList>())
      {
        mType = PropType::SelectableStrings;
        AddSelectableStrings();
      }
    }

    ~PropertyWidget()
    {
      delete mValues;
    }

    T GetPropertyValues();

    std::string GetName()
    {
      return mPropertyName;
    }

    void SetValue(T aVal);

    virtual void SaveToEngine()
    {
      throw "This function shouldn't be called. It is overridden by the ComponentProperty class, to be used there.";
    }

    virtual void ReloadValueFromEngine()
    {
      throw "This function shouldn't be called. It is overridden by the ComponentProperty class, to be used there.";
    }

    std::vector<QWidget*>& GetWidgets()
    {
      return mWidgets;
    }

    QLabel* GetLabelWidget()
    {
      return mLabel;
    }

  private:

    MainWindow *mMainWindow;
    YTE::Property *mProperty;

    std::vector<QWidget*> mWidgets;
    QHBoxLayout *mValues;
    std::string mPropertyName;

    QLabel *mLabel;

    void AddInteger(int aVal = 0);
    void AddFloat(float aVal = 0.0f);
    void AddString(YTE::String aVal = "");
    void AddStdString(std::string aVal = "");
    void AddBool(bool aVal = false);
    void AddSelectableStrings(const QStringList &aStrList = QStringList());

  };

  template<class T>
  inline void PropertyWidget<T>::AddInteger(int aVal)
  {
    LineEdit *widg = new LineEdit(this, mMainWindow);
    QIntValidator *validate = new QIntValidator(std::numeric_limits<int>::min(),
                                                std::numeric_limits<int>::max(),
                                                this);
    widg->setValidator(validate);
    widg->setText(QString(std::to_string(aVal).c_str()));
    mValues->addWidget(widg);
    mWidgets.push_back(widg);
  }

  template<class T>
  inline void PropertyWidget<T>::AddFloat(float aVal)
  {
    LineEdit *widg = new LineEdit(this, mMainWindow);

    QDoubleValidator *validate = new QDoubleValidator(-std::numeric_limits<float>::max(),
                                                      std::numeric_limits<float>::max(),
                                                      7,
                                                      this);
    widg->setValidator(validate);
    char buff[20] = { '\0' };
    sprintf_s(buff, "%.3f", aVal);
    widg->setText(buff);
    mValues->addWidget(widg);
    mWidgets.push_back(widg);
  }

  template<class T>
  inline void PropertyWidget<T>::AddString(YTE::String aVal)
  {
    LineEdit *widg = new LineEdit(this, mMainWindow);
    widg->setText(aVal.c_str());
    mValues->addWidget(widg);
    mWidgets.push_back(widg);
  }

  template<class T>
  inline void PropertyWidget<T>::AddStdString(std::string aVal)
  {
    LineEdit *widg = new LineEdit(this, mMainWindow);
    widg->setText(aVal.c_str());
    mValues->addWidget(widg);
    mWidgets.push_back(widg);
  }

  template<class T>
  inline void PropertyWidget<T>::AddBool(bool aVal)
  {
    CheckBox *widg = new CheckBox(this, mMainWindow);
    widg->setChecked(aVal);
    mValues->addWidget(widg);
    mWidgets.push_back(widg);
  }

  template<class T>
  inline void PropertyWidget<T>::AddSelectableStrings(const QStringList &aStrList)
  {
    QComboBox *widg = new QComboBox(this);
    widg->insertItems(0, aStrList);
    mValues->addWidget(widg);
    mWidgets.push_back(widg);
  }

  template<>
  inline int PropertyWidget<int>::GetPropertyValues()
  {
    LineEdit *deriv = dynamic_cast<LineEdit*>(mWidgets[0]);
    int value = deriv->text().toInt();
    return value;
  }

  template<>
  inline float PropertyWidget<float>::GetPropertyValues()
  {
    LineEdit *deriv = dynamic_cast<LineEdit*>(mWidgets[0]);
    float value = deriv->text().toFloat();
    return value;
  }

  template<>
  inline YTE::String PropertyWidget<YTE::String>::GetPropertyValues()
  {
    LineEdit *deriv = dynamic_cast<LineEdit*>(mWidgets[0]);
    YTE::String value = deriv->text().toStdString();
    return value;
  }

  template<>
  inline std::string PropertyWidget<std::string>::GetPropertyValues()
  {
    LineEdit *deriv = dynamic_cast<LineEdit*>(mWidgets[0]);
    std::string value = deriv->text().toStdString();
    return value;
  }

  template<>
  inline bool PropertyWidget<bool>::GetPropertyValues()
  {
    CheckBox *deriv = dynamic_cast<CheckBox*>(mWidgets[0]);
    bool value = deriv->isChecked();
    return value;
  }

  template<>
  inline glm::vec2 PropertyWidget<glm::vec2>::GetPropertyValues()
  {
    glm::vec2 value;

    LineEdit *w0 = dynamic_cast<LineEdit*>(mWidgets[0]);
    value[0] = w0->text().toFloat();

    LineEdit *w1 = dynamic_cast<LineEdit*>(mWidgets[1]);
    value[1] = w1->text().toFloat();

    return value;
  }

  template<>
  inline glm::vec3 PropertyWidget<glm::vec3>::GetPropertyValues()
  {
    glm::vec3 value;

    if (mProperty->GetAttribute<YTE::EditableColor>())
    {
      auto colorPicker = dynamic_cast<ColorPicker*>(mWidgets[0]);
      auto color = colorPicker->GetColor();
      value = glm::vec3{color.x, color.y, color.z};
    }
    else
    {
      LineEdit *w0 = dynamic_cast<LineEdit*>(mWidgets[0]);
      value[0] = w0->text().toFloat();

      LineEdit *w1 = dynamic_cast<LineEdit*>(mWidgets[1]);
      value[1] = w1->text().toFloat();

      LineEdit * w2 = dynamic_cast<LineEdit*>(mWidgets[2]);
      value[2] = w2->text().toFloat();
    }

    return value;
  }

  template<>
  inline glm::vec4 PropertyWidget<glm::vec4>::GetPropertyValues()
  {
    glm::vec4 value;

    if (mProperty->GetAttribute<YTE::EditableColor>())
    {
      auto colorPicker = dynamic_cast<ColorPicker*>(mWidgets[0]);
      value = colorPicker->GetColor();
    }
    else
    {
      LineEdit *w0 = dynamic_cast<LineEdit*>(mWidgets[0]);
      value[0] = w0->text().toFloat();

      LineEdit *w1 = dynamic_cast<LineEdit*>(mWidgets[1]);
      value[1] = w1->text().toFloat();

      LineEdit *w2 = dynamic_cast<LineEdit*>(mWidgets[2]);
      value[2] = w2->text().toFloat();

      LineEdit *w3 = dynamic_cast<LineEdit*>(mWidgets[3]);
      value[3] = w3->text().toFloat();
    }

    return value;
  }

  template<>
  inline QStringList PropertyWidget<QStringList>::GetPropertyValues()
  {
    QComboBox * deriv = dynamic_cast<QComboBox*>(mWidgets[0]);
    int index = deriv->currentIndex();

    QStringList str;
    str.push_back(deriv->itemText(index));

    return str;
  }

  template<class T>
  inline void PropertyWidget<T>::SetValue(T aVal)
  {
  }


  template<>
  inline void PropertyWidget<float>::SetValue(float aVal)
  {
    LineEdit * widg = dynamic_cast<LineEdit*>(mWidgets[0]);
    char buff[20] = { '\0' };
    sprintf_s(buff, "%.3f", aVal);
    widg->setText(buff);
  }

  template<>
  inline void PropertyWidget<int>::SetValue(int aVal)
  {
    LineEdit * widg = dynamic_cast<LineEdit*>(mWidgets[0]);
    widg->setText(QString(std::to_string(aVal).c_str()));
  }

  template<>
  inline void PropertyWidget<YTE::String>::SetValue(YTE::String aVal)
  {
    LineEdit * widg = dynamic_cast<LineEdit*>(mWidgets[0]);
    widg->setText(aVal.c_str());
  }

  template<>
  inline void PropertyWidget<std::string>::SetValue(std::string aVal)
  {
    LineEdit * widg = dynamic_cast<LineEdit*>(mWidgets[0]);
    widg->setText(aVal.c_str());
  }

  template<>
  inline void PropertyWidget<bool>::SetValue(bool aVal)
  {
    CheckBox * widg = dynamic_cast<CheckBox*>(mWidgets[0]);
    widg->setChecked(aVal);
  }

  template<>
  inline void PropertyWidget<glm::vec2>::SetValue(glm::vec2 aVal)
  {
    for (size_t i = 0; i < mWidgets.size(); ++i)
    {
      LineEdit * widg = dynamic_cast<LineEdit*>(mWidgets[i]);
      char buff[100] = { '\0' };
      sprintf_s(buff, "%.3f", aVal[i]);
      widg->setText(buff);
    }
  }

  template<>
  inline void PropertyWidget<glm::vec3>::SetValue(glm::vec3 aVal)
  {
    if (mProperty->GetAttribute<YTE::EditableColor>())
    {
      auto colorPicker = dynamic_cast<ColorPicker*>(mWidgets[0]);
      colorPicker->SetColor(ToQColor(glm::vec4{ aVal, 1.0f }));
    }
    else
    {
      for (size_t i = 0; i < mWidgets.size(); ++i)
      {
        LineEdit * widg = dynamic_cast<LineEdit*>(mWidgets[i]);
        char buff[100] = { '\0' };
        sprintf_s(buff, "%.6f", aVal[i]);
        widg->setText(buff);
      }
    }
  }

  template<>
  inline void PropertyWidget<glm::vec4>::SetValue(glm::vec4 aVal)
  {
    if (mProperty->GetAttribute<YTE::EditableColor>())
    {
      auto colorPicker = dynamic_cast<ColorPicker*>(mWidgets[0]);
      colorPicker->SetColor(ToQColor(aVal));
    }
    else
    {
      for (size_t i = 0; i < mWidgets.size(); ++i)
      {
        LineEdit * widg = dynamic_cast<LineEdit*>(mWidgets[i]);
        char buff[100] = { '\0' };
        sprintf_s(buff, "%.6f", aVal[i]);
        widg->setText(buff);
      }
    }
  }

  template<>
  inline void PropertyWidget<glm::quat>::SetValue(glm::quat)
  {
  }

  template<>
  inline void PropertyWidget<QStringList>::SetValue(QStringList aStrList)
  {
    QComboBox * widg = dynamic_cast<QComboBox*>(mWidgets[0]);
    widg->clear();
    widg->insertItems(0, aStrList);
  }

}
