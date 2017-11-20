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

#include <qapplication.h>
#include <qlabel.h>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Meta/Type.hpp"
#include "YTE/Meta/Attribute.hpp"

#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentProperty.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/ComponentBrowser/HeaderListProperty.hpp"
#include "YTEditor/ComponentBrowser/HeaderListWidget.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"


namespace YTEditor
{

  ComponentWidget::ComponentWidget(YTE::Type *type,
                                   const char *name,
                                   YTE::Component *engineComp,
                                   MainWindow *aMainWindow,
                                   QTreeWidgetItem *aTopItem,
                                   QWidget *parent) 
    : QFrame(parent)
    , mEngineComponent(engineComp)
    , mType(type)
    , mProperties(new QVBoxLayout(this))
    , mCompName(name)
    , mMainWindow(aMainWindow)
    , mTopItem(aTopItem)
  {
    setLayout(mProperties);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  }

  std::string const& ComponentWidget::GetName() const
  {
    return mCompName;
  }

  void ComponentWidget::RemoveProperty(QWidget * aWidget)
  {
    mProperties->removeWidget(aWidget);
  }


  void ComponentWidget::LoadPropertyMap(YTE::Component *aComponent,
    YTE::OrderedMultiMap<std::string, std::unique_ptr<YTE::Property>>& aProperties,
    bool aProperty)
  {

    // load all properties
    for (auto& prop : aProperties)
    {
      auto editAttrib = prop.second.get()->GetAttribute<YTE::EditorProperty>();

      if (editAttrib && editAttrib->mVisible)
      {
        LoadProperty(*aComponent, aProperty, prop);
      }
    }
  }

  void ComponentWidget::LoadProperties(YTE::Component *aComponent)
  {
    // check if this property contains the EditorHeader attribute
    auto headerAttrib = aComponent->GetType()->GetAttribute<YTE::EditorHeaderList>();

    if (headerAttrib)
    {
      YTE::Animator *animComp = dynamic_cast<YTE::Animator*>(aComponent);

      auto animations = animComp->Lister(aComponent);

      for (auto &anim : animations)
      {
        QTreeWidgetItem *header = new QTreeWidgetItem(mTopItem);
        header->setText(0, anim.second.c_str());

        HeaderListWidget *widg = new HeaderListWidget(anim.first, anim.second, mMainWindow, mEngineComponent, header);

        QTreeWidgetItem *body = new QTreeWidgetItem(header);
        body->setFlags(Qt::NoItemFlags);
        
        mMainWindow->GetComponentBrowser().GetComponentTree()->setItemWidget(body, 0, widg);

        header->addChild(body);
        
        mTopItem->addChild(header);
      }
    }

    auto &propertyMap = aComponent->GetType()->GetProperties();
    auto &fieldMap = aComponent->GetType()->GetFields();

    LoadPropertyMap(aComponent, propertyMap, true);
    LoadPropertyMap(aComponent, fieldMap, false);
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

  void ComponentWidget::LoadProperty(YTE::Component &aComponent, bool aProperty, std::pair<const std::string, std::unique_ptr<YTE::Property>> &aProp)
  {
    auto getter = aProp.second.get()->GetGetter();

    YTE::Any value = getter->Invoke(&aComponent);

    if (value.IsType<int>())
    {
      ComponentProperty<int> *comProp = AddPropertyOrField<int>(aProp.first, aProperty);
      int propData = value.As<int>();
      comProp->SetValue(propData);
      comProp->SetEvents();
    }
    else if (value.IsType<float>())
    {
      ComponentProperty<float> *comProp = AddPropertyOrField<float>(aProp.first, aProperty);
      float propData = value.As<float>();
      comProp->SetValue(propData);
      comProp->SetEvents();
    }
    else if (value.IsType<YTE::String>())
    {
      ComponentProperty<YTE::String> *comProp = AddPropertyOrField<YTE::String>(aProp.first, aProperty);
      YTE::String propData = value.As<YTE::String>();
      comProp->SetValue(propData);
      comProp->SetEvents();
    }
    else if (value.IsType<std::string>())
    {
      // check if it's a drop down of strings
      auto dropDownAttrib = aProp.second.get()->GetAttribute<YTE::DropDownStrings>();

      if (dropDownAttrib)
      {
        // get the property
        ComponentProperty<QStringList> * comProp = AddPropertyOrField<QStringList>(aProp.first, aProperty);

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
            static_cast<QComboBox*>(comProp->GetWidgets()[0])->setCurrentIndex(index);
          }
        }

        comProp->SetEvents();
      }
      else
      {
        // otherwise it's just an editable text field
        ComponentProperty<std::string> * comProp = AddPropertyOrField<std::string>(aProp.first, aProperty);
        std::string propData = value.As<std::string>();
        comProp->SetValue(propData);
        comProp->SetEvents();
      }
    }
    else if (value.IsType<bool>())
    {
      ComponentProperty<bool> *comProp = AddPropertyOrField<bool>(aProp.first, aProperty);
      bool propData = value.As<bool>();
      comProp->SetValue(propData);
      comProp->SetEvents();
    }
    else if (value.IsType<glm::vec2>())
    {
      ComponentProperty<glm::vec2> *comProp = AddPropertyOrField<glm::vec2>(aProp.first, aProperty);
      auto propData = value.As<glm::vec2>();
      comProp->SetValue(propData);
      comProp->SetEvents();
    }
    else if (value.IsType<glm::vec3>())
    {
      ComponentProperty<glm::vec3> *comProp = AddPropertyOrField<glm::vec3>(aProp.first, aProperty);
      auto propData = value.As<glm::vec3>();
      comProp->SetValue(propData);
      comProp->SetEvents();
    }
    else if (value.IsType<glm::vec4>())
    {
      ComponentProperty<glm::vec4> *comProp = AddPropertyOrField<glm::vec4>(aProp.first, aProperty);
      auto propData = value.As<glm::vec4>();
      comProp->SetValue(propData);
      comProp->SetEvents();
    }
  }

  ComponentDelegate::ComponentDelegate(ComponentTree * aTree, QWidget * aParent)
    : QStyledItemDelegate(aParent), mTree(aTree)
  {
  }

  void ComponentDelegate::paint(QPainter * painter,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
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

  bool ComponentDelegate::editorEvent(QEvent * event,
    QAbstractItemModel * model,
    const QStyleOptionViewItem & option,
    const QModelIndex & index)
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

}
