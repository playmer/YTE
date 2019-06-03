/******************************************************************************/
/*!
\file   MaterialViewer.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the material viewer widget.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <qcombobox.h>
#include <qwindow.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Platform/Window.hpp"

#include "YTEditor/ComponentBrowser/PropertyWidget.hpp"
#include "YTEditor/GameWindow/GameWindow.hpp"
#include "YTEditor/MaterialViewer/MaterialViewer.hpp"


namespace YTEditor
{

  MaterialViewer::MaterialViewer(MainWindow *aMainWindow,
                                 QWidget *aParent,
                                 YTE::Window *aWindow)
    : QWidget(aParent)
    , mComboBox(new QComboBox(this))
    , mMaterialWindow(nullptr)
    , mContainer(new QWidget(this))
    , mCurrentSubMeshes(nullptr)
    , mMainWindow(aMainWindow)
  {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    mBaseLayout = new QGridLayout();
    mScrollArea = new QScrollArea();
    mContainerLayout = new QVBoxLayout();
    mContainerLayout->setSpacing(0);

    mMaterialWindow = new SubWindow(aWindow, aMainWindow);
    QWidget *windowContainer = QWidget::createWindowContainer(mMaterialWindow);

    windowContainer->setMinimumHeight(100);
    windowContainer->setMinimumWidth(100);

    mBaseLayout->addWidget(mComboBox);
    mBaseLayout->addWidget(windowContainer);
    mBaseLayout->addWidget(mScrollArea);

    auto id = mMaterialWindow->winId();

    aWindow->SetWindowId(reinterpret_cast<void*>(id));

    this->setLayout(mBaseLayout);

    mContainer->setLayout(mContainerLayout);

    mScrollArea->setWidgetResizable(true);
    mScrollArea->setWidget(mContainer);
    this->setMinimumWidth(300);

    connect(mComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      [=](int index) { this->OnCurrentMaterialChanged(index); });


    auto engine = aMainWindow->GetRunningEngine();

    
    auto renderer = engine->GetComponent<YTE::GraphicsSystem>()->GetRenderer();
    renderer->RegisterWindowForDraw(aWindow);

    YTE::String spaceName{ "MaterialViewer" };
    YTE::Space *space = engine->AddComposition<YTE::Space>(spaceName,
                                                           engine, 
                                                           nullptr);

    space->CreateBlankLevel(spaceName);

    auto view = space->GetComponent<YTE::GraphicsView>();

    std::string windowName{ "MaterialViewer" };
    view->ChangeWindow(windowName);

    space->Initialize();
  }

  MaterialViewer::~MaterialViewer()
  {
  }

  void MaterialViewer::LoadMaterial(YTE::UBOs::Material const &aMaterial)
  {
    YTE::UnusedArguments(aMaterial);

    this->Clear();

    // glm::vec3
    //AddVec3Property("Diffuse", glm::vec3(aMaterial.mDiffuse));
    //AddVec3Property("Ambient", glm::vec3(aMaterial.mAmbient));
    //AddVec3Property("Specular", glm::vec3(aMaterial.mSpecular));
    //AddVec3Property("Emissive", glm::vec3(aMaterial.mEmissive));
    //AddVec3Property("Transparent", glm::vec3(aMaterial.mTransparent));
    //AddVec3Property("Reflective", glm::vec3(aMaterial.mReflective));
    //
    ////float
    //AddFloatProperty("Opacity", aMaterial.mOpacity);
    //AddFloatProperty("Shininess", aMaterial.mShininess);
    //AddFloatProperty("Shiny Strength", aMaterial.mShininessStrength);
    //AddFloatProperty("Reflectivity", aMaterial.mReflectivity);
    //AddFloatProperty("Reflective Index", aMaterial.mReflectiveIndex);
    //AddFloatProperty("Bump Scaling", aMaterial.mBumpScaling);
  }

  void MaterialViewer::SetMaterialsList(std::vector<YTE::Submesh> * aSubMeshList)
  {
    mCurrentSubMeshes = aSubMeshList;

    QStringList matNames;

    for (auto& smesh : *mCurrentSubMeshes)
    {
      matNames.push_back(smesh.mData.mMaterialName.c_str());
    }

    mComboBox->clear();
    mComboBox->insertItems(0, matNames);
  }


  void MaterialViewer::LoadNoMaterial()
  {
    if (mComboBox)
    {
      mComboBox->clear();
    }

    mCurrentSubMeshes = nullptr;

    this->Clear();
  }

  SubWindow* MaterialViewer::GetSubWindow()
  {
    return mMaterialWindow;
  }

  void MaterialViewer::OnCurrentMaterialChanged(int aIndex)
  {
    if (aIndex < mCurrentSubMeshes->size())
    {
      LoadMaterial((*mCurrentSubMeshes)[aIndex].mData.mUBOMaterial);
    }
  }

  //void MaterialViewer::AddVec3Property(const char *aName, glm::vec3 const &aVec)
  //{
  //  PropertyWidget<glm::vec3> * prop = AddProperty<glm::vec3>(aName);
  //  prop->SetValue(aVec);
  //}
  //
  //void MaterialViewer::AddFloatProperty(const char *aName, float aVal)
  //{
  //  PropertyWidget<float> * prop = AddProperty<float>(aName);
  //  prop->SetValue(aVal);
  //}

  void MaterialViewer::Clear()
  {
    ClearLayout();

    while (!mProperties.empty())
    {
      delete mProperties.back();
      mProperties.pop_back();
    }

    mProperties.clear();
  }

  void MaterialViewer::ClearLayout()
  {
    delete mContainerLayout;
    mContainerLayout = new QVBoxLayout();
    mContainer->setLayout(mContainerLayout);
  }

}

