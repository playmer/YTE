#pragma once

#include <qframe.h>
#include <qlayout.h>
#include <qstyleditemdelegate.h>
#include <qtreewidget.h>

#include "YTE/Core/Utilities.hpp"


namespace YTE
{
  class Component;
  class Type;
  class Animator;
}


namespace YTEditor
{

  class MainWindow;

  template <class T>
  class PropertyWidget;

  class PropertyWidgetBase;

  template <class T>
  class HeaderListProperty;

  class ComponentTree;

  class HeaderListWidget : public QFrame
  {
  public:

    HeaderListWidget(YTE::Object *aObject,
                     const std::string &aName,
                     MainWindow *aMainWindow,
                     YTE::Component *aComponent,
                     QTreeWidgetItem *aParent);


    void SavePropertiesToEngine();

    std::vector<PropertyWidgetBase*> GetPropertyWidgets();

    void keyPressEvent(QKeyEvent *aEvent);

    MainWindow* GetMainWindow();

    YTE::Component* GetEngineComponent();

    YTE::Object* GetEngineObject();

    void RemoveSelf();

  private:

    MainWindow *mMainWindow;

    YTE::Component *mEngineComponent;

    YTE::Object *mObject;

    std::string mHeaderName;
    QVBoxLayout *mProperties;

    QTreeWidgetItem *mParentItem;

    std::vector<PropertyWidgetBase*> mPropertyWidgets;

    void LoadProperties(YTE::Object *aObject);

    template <typename tType>
    HeaderListProperty<tType>* AddProperty(const std::string aName, YTE::Property *aProp, YTE::Object *aObject)
    {
      HeaderListProperty<tType> *prop = new HeaderListProperty<tType>(aName, mMainWindow, this, aProp, aObject);
      mProperties->addWidget(prop);
      return prop;
    }

  };


  class HeaderListDelegate : public QStyledItemDelegate
  {
  public:
    HeaderListDelegate(YTE::Animator *aAnimComp, ComponentTree *aTree, QWidget *aParent = nullptr);

    void paint(QPainter *painter,
      const QStyleOptionViewItem &option,
      const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
      QAbstractItemModel *model,
      const QStyleOptionViewItem &option,
      const QModelIndex &index);

  private:

    ComponentTree *mTree;
    YTE::Animator *mAnimator;

  };

}