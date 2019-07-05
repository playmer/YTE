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

#include "YTEditor/Framework/ForwardDeclarations.hpp"
#include "YTEditor/Framework/Widget.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

class QVBoxLayout;
class QWidget;
class QComboBox;
class QWindow;

namespace YTEditor
{
  class MaterialViewer : public Framework::Widget
  {
  public:
    MaterialViewer(YTELevelEditor* aLevelEditor);

    void LoadMaterial(YTE::UBOs::Material const& aMaterial);

    void SetMaterialsList(std::vector<YTE::Submesh> * aSubMeshList);

    void LoadNoMaterial();

    SubWindow* GetSubWindow();

    static std::string GetName();

    ToolWindowManager::AreaReference GetToolArea() override;

  private:

    void OnCurrentMaterialChanged(int aIndex);

    // Add, Get, or Remove a property
    template <typename tType>
    PropertyWidget<tType>* AddProperty(const char *aName, YTE::Property *aProp)
    {
      PropertyWidget<tType> *prop = new PropertyWidget<tType>(aName, aProp, mLevelEditor, mContainerLayout->widget());
      mContainerLayout->addWidget(prop);
      mContainer->setLayout(mContainerLayout);
      mProperties.push_back(prop);
      return prop;
    }

    //void AddVec3Property(const char * aName, glm::vec3 const& aVec);
    //void AddFloatProperty(const char * aName, float aVal);

    void Clear();
    void ClearLayout();

    YTELevelEditor* mLevelEditor;

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