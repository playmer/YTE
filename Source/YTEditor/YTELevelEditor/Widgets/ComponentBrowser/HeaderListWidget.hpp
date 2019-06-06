#pragma once

#include <qframe.h>
#include <qlayout.h>
#include <qstyleditemdelegate.h>
#include <qtreewidget.h>

#include "YTE/Core/Utilities.hpp"

#include "YTEditor/Framework/ForwardDeclarations.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{
  class HeaderListWidget : public QFrame
  {
  public:

    HeaderListWidget(YTE::Object *aObject,
                     const std::string &aName,
                     Framework::MainWindow *aMainWindow,
                     YTE::Component *aComponent,
                     QTreeWidgetItem *aParent);


    void SavePropertiesToEngine();

    std::vector<PropertyWidgetBase*> GetPropertyWidgets();

    void keyPressEvent(QKeyEvent *aEvent);

    Framework::MainWindow* GetMainWindow();

    YTE::Component* GetEngineComponent();

    YTE::Object* GetEngineObject();

    void RemoveSelf();

  private:

    Framework::MainWindow* mMainWindow;

    YTE::Component* mEngineComponent;

    YTE::Object* mObject;

    std::string mHeaderName;
    QVBoxLayout* mProperties;

    QTreeWidgetItem* mParentItem;

    std::vector<PropertyWidgetBase*> mPropertyWidgets;

    void LoadProperties(YTE::Object* aObject);

    template <typename tType>
    HeaderListProperty<tType>* AddProperty(const std::string aName, YTE::Property* aProperty, YTE::Object* aObject)
    {
      HeaderListProperty<tType> *prop = new HeaderListProperty<tType>(aName, mMainWindow->GetWorkspace<YTELevelEditor>(), this, aProperty, aObject);
      mProperties->addWidget(prop);
      return prop;
    }

  };


  class HeaderListDelegate : public QStyledItemDelegate
  {
  public:
    HeaderListDelegate(YTE::Animator* aAnimComponent, ComponentTree* aTree, QWidget* aParent = nullptr);

    void paint(QPainter* aPainter,
      QStyleOptionViewItem const& aOption,
      QModelIndex const& aIndex) const override;

    bool editorEvent(QEvent* aEvent,
      QAbstractItemModel* aModel,
      QStyleOptionViewItem const& aOption,
      QModelIndex const& aIndex);

  private:

    ComponentTree* mTree;
    YTE::Animator* mAnimator;

  };

}