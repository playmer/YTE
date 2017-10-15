/******************************************************************************/
/*!
\file   PropertyWidgetBase.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The base class for PropertyWidget.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qwidget.h>

class PropertyWidgetBase : public QWidget
{
public:

  PropertyWidgetBase(QWidget * parent = nullptr) : QWidget(parent), mType(PropType::Int) { }

  int GetBaseType() const 
  { 
    return mType;
  };

  virtual void SaveToEngine() {};

  enum PropType
  {
    Int,
    Float,
    String,
    StdString,
    Bool,
    Vec2,
    Vec3,
    Vec4,
    Quaternion,
    SelectableStrings
  };

protected:
  int mType;
};