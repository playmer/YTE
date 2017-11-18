

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

  void HeaderListWidget::RemoveSelf()
  {
    mParentItem->treeWidget()->removeItemWidget(mParentItem, 0);
  }

  void HeaderListWidget::LoadProperties(YTE::Object *aObject)
  {
    auto *type = aObject->GetType();
    auto &propMap = type->GetProperties();

    for (auto &p : propMap)
    {
      QTreeWidgetItem *propItem = new QTreeWidgetItem();

      QFrame *frame = new QFrame();
      QVBoxLayout *layout = new QVBoxLayout(frame);

      auto getter = p.second.get()->GetGetter();

      YTE::Any value = getter->Invoke(aObject);

      if (value.IsType<int>())
      {
        HeaderListProperty<int> *headProp = AddProperty<int>(p.first, p.second.get());
        int propData = value.As<int>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<float>())
      {
        HeaderListProperty<float> *headProp = AddProperty<float>(p.first, p.second.get());
        float propData = value.As<float>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<YTE::String>())
      {
        HeaderListProperty<YTE::String> *headProp = AddProperty<YTE::String>(p.first, p.second.get());
        YTE::String propData = value.As<YTE::String>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<std::string>())
      {
        // check if it's a drop down of strings
        auto dropDownAttrib = p.second.get()->GetAttribute<YTE::DropDownStrings>();

        if (dropDownAttrib)
        {
          // get the property
          HeaderListProperty<QStringList> * headProp = AddProperty<QStringList>(p.first, p.second.get());

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
          HeaderListProperty<std::string> * headProp = AddProperty<std::string>(p.first, p.second.get());
          std::string propData = value.As<std::string>();
          headProp->SetValue(propData);
          headProp->SetEvents();
        }
      }
      else if (value.IsType<bool>())
      {
        HeaderListProperty<bool> *headProp = AddProperty<bool>(p.first, p.second.get());
        bool propData = value.As<bool>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<glm::vec2>())
      {
        HeaderListProperty<glm::vec2> *headProp = AddProperty<glm::vec2>(p.first, p.second.get());
        glm::vec2 propData = value.As<glm::vec2>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<glm::vec3>())
      {
        HeaderListProperty<glm::vec3> *headProp = AddProperty<glm::vec3>(p.first, p.second.get());
        glm::vec3 propData = value.As<glm::vec3>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
      else if (value.IsType<glm::vec4>())
      {
        HeaderListProperty<glm::vec4> *headProp = AddProperty<glm::vec4>(p.first, p.second.get());
        glm::vec4 propData = value.As<glm::vec4>();
        headProp->SetValue(propData);
        headProp->SetEvents();
      }
    }

  }

  HeaderListDelegate::HeaderListDelegate(YTE::Animator *aAnimComp, ComponentTree *aTree, QWidget *aParent)
    : QStyledItemDelegate(aParent), mTree(aTree), mAnimator(aAnimComp)
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

  bool HeaderListDelegate::editorEvent(QEvent *event,
    QAbstractItemModel *model,
    const QStyleOptionViewItem &option,
    const QModelIndex &index)
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

