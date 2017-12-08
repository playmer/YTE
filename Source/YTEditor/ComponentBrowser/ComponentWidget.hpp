/******************************************************************************/
/*!
\file   ComponentWidget.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The widget containing all properties of a component. 
Displayed in the ComponentTree.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

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
}


namespace YTEditor
{

  class MainWindow;

  template <class T>
  class PropertyWidget;

  class PropertyWidgetBase;

  template <class T>
  class ComponentProperty;

  class ComponentTree;


  class ComponentWidget : public QFrame
  {
  public:

    ComponentWidget(YTE::Type *type,
                    const char *name,
                    YTE::Component *engineComp,
                    MainWindow *aMainWindow,
                    QTreeWidgetItem *aTopItem,
                    QWidget *parent = nullptr);


    std::string const& GetName() const;

    // Add, Get, or Remove a property from the component
    template <typename tType>
    ComponentProperty<tType>* AddPropertyOrField(std::pair<const std::string, std::unique_ptr<YTE::Property>> &aProp, bool aProperty)
    {
      ComponentProperty<tType> *prop = new ComponentProperty<tType>(aProp, mMainWindow, this);
      mProperties->addWidget(prop);

      if (aProperty)
      {
        mPropertyWidgets.push_back(prop);
      }
      else
      {
        mFieldWidgets.push_back(prop);
      }
      return prop;
    }


    void RemoveProperty(QWidget *aWidget);

    void LoadProperties(YTE::Component *aComponent);
    void LoadPropertyMap(YTE::Component *aComponent,
                         YTE::OrderedMultiMap<std::string, std::unique_ptr<YTE::Property>>& aProperties,
                         bool aProperty);

    void SavePropertiesToEngine();

    YTE::Type* GetType()
    {
      return mType;
    }

    YTE::Component* GetEngineComponent()
    {
      return mEngineComponent;
    }

    void RemoveComponentFromEngine();

    std::vector<PropertyWidgetBase*> GetPropertyWidgets();
    std::vector<PropertyWidgetBase*> GetFieldWidgets();

    void keyPressEvent(QKeyEvent *aEvent);

    MainWindow* GetMainWindow() { return mMainWindow; }

    QTreeWidgetItem* GetParentItem();

  private:

    MainWindow *mMainWindow;

    QTreeWidgetItem *mTopItem;

    std::string mCompName;
    QVBoxLayout * mProperties;
    YTE::Type * mType;

    YTE::Component * mEngineComponent;

    std::vector<PropertyWidgetBase*> mPropertyWidgets;
    std::vector<PropertyWidgetBase*> mFieldWidgets;

    void LoadProperty(YTE::Component &aComponent, 
                      bool aProperty, 
                      std::pair<const std::string, std::unique_ptr<YTE::Property>> &aProp);


    void LoadEditorHeader();

  };


  class ComponentDelegate : public QStyledItemDelegate
  {
  public:
    ComponentDelegate(ComponentTree *aTree, QWidget *aParent = nullptr);

    void paint(QPainter *painter,
      const QStyleOptionViewItem &option,
      const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
      QAbstractItemModel *model,
      const QStyleOptionViewItem &option,
      const QModelIndex &index);

  private:
    ComponentTree *mTree;

  };

}