/******************************************************************************/
/*!
\file   MaterialViewer.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The widget for viewing all the properties of the material of the 
currently selected object.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qscrollarea.h>
#include <qlayout.h>

#include "glm/glm.hpp"

#include "YTE/Graphics/InstantiatedMesh.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Utilities/String/String.h"


class YTEditorMainWindow;
class QVBoxLayout;
class QWidget;
class QComboBox;
class SubWindow;
class QWindow;

template <class T>
class PropertyWidget;
class PropertyWidgetBase;


class MaterialViewer : public QWidget
{
public:
  MaterialViewer(YTEditorMainWindow *mainWindow, 
                 QWidget *parent = nullptr, 
                 YTE::Window *aWindow = nullptr);
  ~MaterialViewer();

  void LoadMaterial(YTE::UBOMaterial const& aMaterial);

  void SetMaterialsList(std::vector<YTE::Mesh::SubMesh> * aSubMeshList);

  void LoadNoMaterial();

private:

  void OnCurrentMaterialChanged(int aIndex);

  // Add, Get, or Remove a property
  template <typename TYPE>
  PropertyWidget<TYPE> * AddProperty(const char * aName)
  {
    PropertyWidget<TYPE> * prop = new PropertyWidget<TYPE>(aName, mMainWindow, mContainerLayout->widget());
    mContainerLayout->addWidget(prop);
    mContainer->setLayout(mContainerLayout);
    mProperties.push_back(prop);
    return prop;
  }

  void AddVec3Property(const char * aName, glm::vec3 const& aVec);
  void AddFloatProperty(const char * aName, float aVal);

  void Clear();
  void ClearLayout();

  YTEditorMainWindow *mMainWindow;

  QComboBox *mComboBox;
  SubWindow *mMaterialWindow;

  QGridLayout *mBaseLayout;
  QScrollArea *mScrollArea;
  QWidget *mContainer;
  QVBoxLayout *mContainerLayout;

  std::vector<PropertyWidgetBase*> mProperties;
  YTE::String mCurrentMaterialName;

  std::vector<YTE::Mesh::SubMesh> *mCurrentSubMeshes;
};