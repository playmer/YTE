

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Meta/Type.hpp"
#include "YTE/Meta/Attribute.hpp"
#include "YTE/Core/Utilities.hpp"


#include "YTEditor/ComponentBrowser/HeaderListWidget.hpp"

#include "YTEditor/ComponentBrowser/PropertyWidgetBase.hpp"
#include "YTEditor/ComponentBrowser/PropertyWidget.hpp"
#include "YTEditor/ComponentBrowser/HeaderListProperty.hpp"

#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "HeaderListWidget.hpp"

namespace YTEditor
{


  HeaderListWidget::HeaderListWidget(YTE::Object *aObject,
                                     const std::string &aName,
                                     MainWindow *aMainWindow,
                                     YTE::Component *aComponent,
                                     QTreeWidgetItem *aParent)
    : mMainWindow(aMainWindow)
    , mEngineComponent(aComponent)
    , mObject(aObject)
    , mHeaderName(aName)
    , mProperties(new QVBoxLayout(this))
    , mParentItem(aParent)
  {
    setLayout(mProperties);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    LoadProperties(aObject);
  }

  void HeaderListWidget::SavePropertiesToEngine()
  {
    for (PropertyWidgetBase* c : mPropertyWidgets)
    {
      c->SaveToEngine();
    }
  }

  std::vector<PropertyWidgetBase*> HeaderListWidget::GetPropertyWidgets()
  {
    return mPropertyWidgets;
  }

  void HeaderListWidget::keyPressEvent(QKeyEvent *aEvent)
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

  MainWindow* HeaderListWidget::GetMainWindow()
  {
    return mMainWindow;
  }

  YTE::Component * HeaderListWidget::GetEngineComponent()
  {
    return mEngineComponent;
  }

  YTE::Object * HeaderListWidget::GetEngineObject()
  {
    return mObject;
  }

  void HeaderListWidget::RemoveSelf()
  {
    delete mObject;
    mParentItem->treeWidget()->removeItemWidget(mParentItem, 0);
  }

  void HeaderListWidget::LoadProperties(YTE::Object *aObject)
  {
    auto *type = aObject->GetType();
    auto &propMap = type->GetProperties();

    for (auto &prop : propMap)
    {
      auto getter = prop.second.get()->GetGetter();

      YTE::Any value = getter->Invoke(aObject);

      if (value.IsType<int>())
      {
        HeaderListProperty<int> *headProp = AddProperty<int>(prop.first, prop.second.get(), aObject);
        int propData = value.As<int>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<float>())
      {
        HeaderListProperty<float> *headProp = AddProperty<float>(prop.first, prop.second.get(), aObject);
        float propData = value.As<float>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<YTE::String>())
      {
        HeaderListProperty<YTE::String> *headProp = AddProperty<YTE::String>(prop.first, prop.second.get(), aObject);
        YTE::String propData = value.As<YTE::String>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<std::string>())
      {
        // check if it's a drop down of strings
        auto dropDownAttrib = prop.second.get()->GetAttribute<YTE::DropDownStrings>();

        if (dropDownAttrib)
        {
          // get the property
          HeaderListProperty<QStringList> * headProp = AddProperty<QStringList>(prop.first, prop.second.get(), aObject);

          // grab the list of strings from the attribute
          std::vector<std::string> strList = (*dropDownAttrib->GetStringGettor())(mEngineComponent);

          // build a QStringList from the vector of strings
          QStringList argStrs;
          for (std::string str : strList)
          {
            argStrs.push_back(str.c_str());
          }

          // set the list of strings on the property
          headProp->SetValue(argStrs);

          if (mPropertyWidgets[0])
          {
            // get the current string
            std::string propData = value.As<std::string>();

            if (!propData.empty())
            {
              int index = argStrs.indexOf(propData.c_str());

              // set the current string of text for the drop down
              dynamic_cast<QComboBox*>(headProp->GetWidgets()[0])->setCurrentIndex(index);
            }
          }

          headProp->SetEvents();
        }
        else
        {
          // otherwise it's just an editable text field
          HeaderListProperty<std::string> * headProp = AddProperty<std::string>(prop.first, prop.second.get(), aObject);
          std::string propData = value.As<std::string>();
          headProp->SetValue(propData);
          headProp->SetEvents();
        }
      }
      else if (value.IsType<bool>())
      {
        HeaderListProperty<bool> *headProp = AddProperty<bool>(prop.first, prop.second.get(), aObject);
        bool propData = value.As<bool>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<glm::vec2>())
      {
        HeaderListProperty<glm::vec2> *headProp = AddProperty<glm::vec2>(prop.first, prop.second.get(), aObject);
        glm::vec2 propData = value.As<glm::vec2>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<glm::vec3>())
      {
        HeaderListProperty<glm::vec3> *headProp = AddProperty<glm::vec3>(prop.first, prop.second.get(), aObject);
        glm::vec3 propData = value.As<glm::vec3>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<glm::vec4>())
      {
        HeaderListProperty<glm::vec4> *headProp = AddProperty<glm::vec4>(prop.first, prop.second.get(), aObject);
        glm::vec4 propData = value.As<glm::vec4>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
    }

  }

  HeaderListDelegate::HeaderListDelegate(YTE::Animator *aAnimComp,
                                         ComponentTree *aTree,
                                         QWidget *aParent)
    : QStyledItemDelegate(aParent)
    , mTree(aTree)
    , mAnimator(aAnimComp)
  {

  }

  void HeaderListDelegate::paint(QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
  {
    Q_UNUSED(index);

    // paint the default QTreeWidgetItem
    QStyledItemDelegate::paint(painter, option, index);

    if (index.parent().isValid())
    {
      return;
    }


    QRect rect = option.rect;

    // get dimensions and top left corner of button
    int x = rect.right() - 30;
    int y = rect.top();
    int w = 30;
    int h = rect.height();

    // fill out button style (visual representation only)
    QStyleOptionButton button;
    button.rect = QRect(x, y, w, h);
    button.text = "X";
    button.state = QStyle::State_Enabled;

    // draw the button
    QApplication::style()->drawControl(QStyle::CE_PushButton,
      &button, painter);
  }

  bool HeaderListDelegate::editorEvent(QEvent *event,
    QAbstractItemModel *model,
    const QStyleOptionViewItem &option,
    const QModelIndex &index)
  {
    Q_UNUSED(index);
    Q_UNUSED(model);

    if (event->type() == QEvent::MouseButtonRelease)
    {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

      QTreeWidgetItem *item = mTree->itemAt(mouseEvent->pos());

      if (!item || item->childCount() == 0)
      {
        return false;
      }

      int clickX = mouseEvent->x();
      int clickY = mouseEvent->y();

      //getting the rect of the cell
      QRect r = option.rect;

      //the X coordinate
      int x = r.left() + r.width() - 30;
      
      //the Y coordinate
      int y = r.top();
      
      //button width
      int w = 30;

      //button height
      int h = 30;

      if (clickX > x && clickX < x + w)
      {
        if (clickY > y && clickY < y + h)
        {
          mTree->RemoveComponent(item);

          return true;
        }
      }
    }

    return false;
  }
}

