/******************************************************************************/
/*!
\file   ComponentWidget.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the ComponentWidget that contains a component's properties
and displays them in the ComponentTree.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "ComponentWidget.hpp"
#include "ComponentProperty.hpp"
#include "YTE/Core/Component.hpp"
#include <QtWidgets/qlabel.h>
#include "YTE/Meta/Type.hpp"
#include "YTE/Meta/Attribute.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTEditorMainWindow.hpp"
#include "ComponentTree.hpp"
#include <qapplication.h>
#include "ComponentBrowser.hpp"

ComponentWidget::ComponentWidget(YTE::Type *type, const char *name, YTE::Component *engineComp, YTEditorMainWindow *aMainWindow, QWidget *parent)
  : QFrame(parent),
    mEngineComponent(engineComp),
    mType(type),
    mProperties(new QVBoxLayout(this)),
    mCompName(name),
    mMainWindow(aMainWindow)
{
  this->setLayout(mProperties);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

ComponentWidget::~ComponentWidget()
{
}

std::string const& ComponentWidget::GetName() const
{
  return mCompName;
}

void ComponentWidget::RemoveProperty(QWidget * aWidget)
{
  mProperties->removeWidget(aWidget);
}

void ComponentWidget::LoadProperties(YTE::Component & aComponent)
{
  auto& propMap = aComponent.GetType()->GetProperties();

  // load all properties
  for (auto& prop : propMap)
  {
    auto editAttrib = prop.second.get()->GetAttribute<YTE::EditorProperty>();

    // check if this property contains the editor attribute
    if (!editAttrib)
    {
      continue;
    }

    auto getter = prop.second.get()->GetGetter();

    YTE::Any value = getter->Invoke(&aComponent);

    if (value.IsType<int>())
    {
      ComponentProperty<int> * comProp = this->AddProperty<int>(prop.first.c_str());
      int propData = value.As<int>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<float>())
    {
      ComponentProperty<float> * comProp = this->AddProperty<float>(prop.first.c_str());
      float propData = value.As<float>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<YTE::String>())
    {
      ComponentProperty<YTE::String> * comProp = this->AddProperty<YTE::String>(prop.first.c_str());
      YTE::String propData = value.As<YTE::String>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<std::string>())
    {
      // check if it's a drop down of strings
      auto dropDownAttrib = prop.second.get()->GetAttribute<YTE::DropDownStrings>();

      if (dropDownAttrib)
      {
        // get the property
        ComponentProperty<QStringList> * comProp = this->AddProperty<QStringList>(prop.first.c_str());

        // grab the list of strings from the attribute
        std::vector<std::string> strList = (*dropDownAttrib->GetStringGettor())(this->mEngineComponent);

        // build a QStringList from the vector of strings
        QStringList argStrs;
        for (std::string str : strList)
        {
          argStrs.push_back(str.c_str());
        }

        // set the list of strings on the property
        comProp->SetValue(argStrs);

        if (mPropertyWidgets[0])
        {
          // get the current string
          std::string propData = value.As<std::string>();

          if (!propData.empty())
          {
            int index = argStrs.indexOf(propData.c_str());

            // set the current string of text for the drop down
            dynamic_cast<QComboBox*>(comProp->GetWidgets()[0])->setCurrentIndex(index);
          }
        }
      }
      else
      {
        // otherwise it's just an editable text field
        ComponentProperty<std::string> * comProp = this->AddField<std::string>(prop.first.c_str());
        std::string propData = value.As<std::string>();
        comProp->SetValue(propData);
      }
    }
    else if (value.IsType<bool>())
    {
      ComponentProperty<bool> * comProp = this->AddProperty<bool>(prop.first.c_str());
      bool propData = value.As<bool>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<glm::vec2>())
    {
      ComponentProperty<glm::vec2> * comProp = this->AddProperty<glm::vec2>(prop.first.c_str());
      auto propData = value.As<glm::vec2>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<glm::vec3>())
    {
      ComponentProperty<glm::vec3> * comProp = this->AddProperty<glm::vec3>(prop.first.c_str());
      auto propData = value.As<glm::vec3>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<glm::vec4>())
    {
      ComponentProperty<glm::vec4> * comProp = this->AddProperty<glm::vec4>(prop.first.c_str());
      auto propData = value.As<glm::vec4>();
      comProp->SetValue(propData);
    }
  }

  auto& fieldMap = aComponent.GetType()->GetFields();

  // load all fields
  for (auto& field : fieldMap)
  {
    auto editAttrib = field.second.get()->GetAttribute<YTE::EditorProperty>();

    // check if this field contains the editor attribute
    if (!editAttrib)
    {
      return;
    }

    auto getter = field.second.get()->GetGetter();

    YTE::Any value = getter->Invoke(&aComponent);

    if (value.IsType<int>())
    {
      ComponentProperty<int> * comProp = this->AddField<int>(field.first.c_str());
      int propData = value.As<int>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<float>())
    {
      ComponentProperty<float> * comProp = this->AddField<float>(field.first.c_str());
      float propData = value.As<float>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<YTE::String>())
    {
      ComponentProperty<YTE::String> * comProp = this->AddField<YTE::String>(field.first.c_str());
      YTE::String propData = value.As<YTE::String>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<std::string>())
    {
      // check if it's a drop down of strings
      auto dropDownAttrib = field.second.get()->GetAttribute<YTE::DropDownStrings>();

      if (dropDownAttrib)
      {
        // get the property
        ComponentProperty<QStringList> * comProp = this->AddField<QStringList>(field.first.c_str());

        // grab the list of strings from the attribute
        std::vector<std::string> strList = (*dropDownAttrib->GetStringGettor())(this->mEngineComponent);

        // build a QStringList from the vector of strings
        QStringList argStrs;
        for (std::string str : strList)
        {
          argStrs.push_back(str.c_str());
        }

        // set the list of strings on the property
        comProp->SetValue(argStrs);

        if (mFieldWidgets[0])
        {
          // get the current string
          std::string propData = value.As<std::string>();
         
          // set the current string of text for the drop down
          dynamic_cast<QComboBox*>(mFieldWidgets[0])->setCurrentText(propData.c_str());
        }
        else
        {
          dynamic_cast<QComboBox*>(mFieldWidgets[0])->setCurrentIndex(0);
        }
      }
      else
      {
        // otherwise it's just an editable text field
        ComponentProperty<std::string> * comProp = this->AddField<std::string>(field.first.c_str());
        std::string propData = value.As<std::string>();
        comProp->SetValue(propData);
      }
    }
    else if (value.IsType<bool>())
    {
      ComponentProperty<bool> * comProp = this->AddField<bool>(field.first.c_str());
      bool propData = value.As<bool>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<glm::vec2>())
    {
      ComponentProperty<glm::vec2> * comProp = this->AddField<glm::vec2>(field.first.c_str());
      auto propData = value.As<glm::vec2>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<glm::vec3>())
    {
      ComponentProperty<glm::vec3> * comProp = this->AddField<glm::vec3>(field.first.c_str());
      auto propData = value.As<glm::vec3>();
      comProp->SetValue(propData);
    }
    else if (value.IsType<glm::vec4>())
    {
      ComponentProperty<glm::vec4> * comProp = this->AddField<glm::vec4>(field.first.c_str());
      auto propData = value.As<glm::vec4>();
      comProp->SetValue(propData);
    }
  }
}

void ComponentWidget::SavePropertiesToEngine()
{
  for (PropertyWidgetBase* c : mPropertyWidgets)
  {
    c->SaveToEngine();
  }

  for (PropertyWidgetBase* c : mFieldWidgets)
  {
    c->SaveToEngine();
  }
}

void ComponentWidget::RemoveComponentFromEngine()
{
  if (!mEngineComponent)
  {
    return;
  }

  YTE::Composition * object = mEngineComponent->GetOwner();

  if (!object)
  {
    return;
  }

 object->RemoveComponent(mEngineComponent->GetType());
 mEngineComponent = nullptr;
}

std::vector<PropertyWidgetBase*> ComponentWidget::GetPropertyWidgets()
{
  return mPropertyWidgets;
}

std::vector<PropertyWidgetBase*> ComponentWidget::GetFieldWidgets()
{
  return mFieldWidgets;
}

void ComponentWidget::keyPressEvent(QKeyEvent * aEvent)
{
  if (aEvent->modifiers() == Qt::Modifier::CTRL && (aEvent->key() == Qt::Key_Z || aEvent->key() == Qt::Key_Y))
  {
    mMainWindow->keyPressEvent(aEvent);
  }
  else
  {
    this->QFrame::keyPressEvent(aEvent);
  }
}

ComponentDelegate::ComponentDelegate(ComponentTree * aTree, QWidget * aParent)
  : QStyledItemDelegate(aParent), mTree(aTree)
{
}

void ComponentDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  Q_UNUSED(index);

  // paint the default QTreeWidgetItem
  QStyledItemDelegate::paint(painter, option, index);

  if (index.parent().isValid())
  {
    return;
  }


  QRect r = option.rect;

  // get dimensions and top left corner of button
  int x = r.right() - 30;
  int y = r.top();
  int w = 30;
  int h = r.height();

  // fill out button style (visual representation only)
  QStyleOptionButton button;
  button.rect = QRect(x, y, w, h);
  button.text = "X";
  button.state = QStyle::State_Enabled;

  // draw the button
  QApplication::style()->drawControl(QStyle::CE_PushButton,
    &button, painter);
}

bool ComponentDelegate::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index)
{
  Q_UNUSED(index);
  Q_UNUSED(model);

  if (event->type() == QEvent::MouseButtonRelease)
  {
    QMouseEvent * e = (QMouseEvent *)event;

    QTreeWidgetItem *item = mTree->itemAt(e->pos());

    if (!item || item->childCount() == 0)
    {
      return false;
    }

    int clickX = e->x();
    int clickY = e->y();

    QRect r = option.rect;//getting the rect of the cell
    int x, y, w, h;
    x = r.left() + r.width() - 30;//the X coordinate
    y = r.top();//the Y coordinate
    w = 30;//button width
    h = 30;//button height

    if (clickX > x && clickX < x + w)
      if (clickY > y && clickY < y + h)
      {
        

        mTree->RemoveComponent(item);
        
        return true;
      }
  }

  return false;
}
