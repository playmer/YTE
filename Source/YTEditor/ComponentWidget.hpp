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

#include <QtWidgets/qframe.h>
#include <QtWidgets/qlayout.h>

#include <qstyleditemdelegate.h>

template <class T>
class PropertyWidget;

class PropertyWidgetBase;

template <class T>
class ComponentProperty;

class ComponentTree;

class YTEditorMainWindow;

namespace YTE
{
  class Component;
  class Type;
}

class ComponentWidget : public QFrame
{
public:

  ComponentWidget(YTE::Type * type, const char * name, 
                  YTE::Component * engineComp, 
                  YTEditorMainWindow *aMainWindow, 
                  QWidget * parent = nullptr);

  ~ComponentWidget();

  std::string const& GetName() const;

  // Add, Get, or Remove a property from the component
  template <typename TYPE>
  ComponentProperty<TYPE> * AddProperty(const char * aName)
  {
    ComponentProperty<TYPE> * prop = new ComponentProperty<TYPE>(aName, mMainWindow, this);
    mProperties->addWidget(prop);
    mPropertyWidgets.push_back(prop);
    return prop;
  }


  // Add, Get, or Remove a field from the component
  template <typename TYPE>
  ComponentProperty<TYPE> * AddField(const char * aName)
  {
    ComponentProperty<TYPE> * prop = new ComponentProperty<TYPE>(aName, mMainWindow, this);
    mProperties->addWidget(prop);
    mFieldWidgets.push_back(prop);
    return prop;
  }

  void RemoveProperty(QWidget * aWidget);

  void LoadProperties(YTE::Component & aComponent);

  void SavePropertiesToEngine();

  YTE::Type * GetType()
  {
    return mType;
  }

  YTE::Component * GetEngineComponent()
  {
    return mEngineComponent;
  }

  void RemoveComponentFromEngine();

  std::vector<PropertyWidgetBase*> GetPropertyWidgets();
  std::vector<PropertyWidgetBase*> GetFieldWidgets();

  void keyPressEvent(QKeyEvent *aEvent);

  YTEditorMainWindow* GetMainWindow() { return mMainWindow; }

private:

  YTEditorMainWindow *mMainWindow;

  std::string mCompName;
  QVBoxLayout * mProperties;
  YTE::Type * mType;

  YTE::Component * mEngineComponent;

  std::vector<PropertyWidgetBase*> mPropertyWidgets;
  std::vector<PropertyWidgetBase*> mFieldWidgets;

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
