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

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Utilities/String/String.hpp"


class QVBoxLayout;
class QWidget;
class QComboBox;
class QWindow;

namespace YTEditor
{

  class MainWindow;
  class SubWindow;

  template <class T>
  class PropertyWidget;
  class PropertyWidgetBase;


  //class MaterialView : public YTE::Component
  //{
  //public:
  //  YTEDeclareType(MaterialView);
  //  MaterialView(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties);
  //  ~MaterialView();
  //
  //  void Initialize() override;
  //  void CreateMaterial();
  //
  //  void CreateTransform();
  //
  //  void TransformUpdate(YTE::TransformChanged *aEvent);
  //
  //private:
  //  YTE::Renderer *mRenderer;
  //  YTE::Window *mWindow;
  //  YTE::Transform *mTransform;
  //  YTE::UBOModel mUBOModel;
  //
  //  std::unique_ptr<YTE::InstantiatedModel> mInstantiatedSkybox;
  //  std::string mTextureName;
  //  YTE::u32 mSubdivisions;
  //  bool mConstructing = true;
  //};


  class MaterialViewer : public QWidget
  {
  public:
    MaterialViewer(MainWindow *mainWindow,
      QWidget *parent = nullptr,
      YTE::Window *aWindow = nullptr);
    ~MaterialViewer();

    void LoadMaterial(YTE::UBOMaterial const& aMaterial);

    void SetMaterialsList(std::vector<YTE::Submesh> * aSubMeshList);

    void LoadNoMaterial();

    SubWindow* GetSubWindow();

  private:

    void OnCurrentMaterialChanged(int aIndex);

    // Add, Get, or Remove a property
    template <typename tType>
    PropertyWidget<tType>* AddProperty(const char *aName, YTE::Property *aProp)
    {
      PropertyWidget<tType> *prop = new PropertyWidget<tType>(aName, aProp, mMainWindow, mContainerLayout->widget());
      mContainerLayout->addWidget(prop);
      mContainer->setLayout(mContainerLayout);
      mProperties.push_back(prop);
      return prop;
    }

    //void AddVec3Property(const char * aName, glm::vec3 const& aVec);
    //void AddFloatProperty(const char * aName, float aVal);

    void Clear();
    void ClearLayout();

    MainWindow *mMainWindow;

    QComboBox *mComboBox;
    SubWindow *mMaterialWindow;

    QGridLayout *mBaseLayout;
    QScrollArea *mScrollArea;
    QWidget *mContainer;
    QVBoxLayout *mContainerLayout;

    std::vector<PropertyWidgetBase*> mProperties;
    YTE::String mCurrentMaterialName;

    std::vector<YTE::Submesh> *mCurrentSubMeshes;
  };

}