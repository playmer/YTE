#include <filesystem>

#include <qpushbutton.h>
#include <qdir.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"

namespace YTEditor
{

  GizmoToolbar::GizmoToolbar(MainWindow * aMainWindow) : QToolBar(aMainWindow), mMainWindow(aMainWindow)
  {

    std::experimental::filesystem::path workingDir{ YTE::Path::GetEnginePath().String() };
    std::experimental::filesystem::path assetsDir{ workingDir.append("CreativeCommons_Icons/")};

    mSelect = new QPushButton(this);
    mSelect->setIcon(QIcon(QDir((assetsDir.generic_string() + "select.png").c_str()).path()));
    mSelect->setIconSize(QSize(20, 20));
    mSelect->setCheckable(true);
    this->connect(mSelect, &QPushButton::toggled, this, &GizmoToolbar::SelectToggled);
    this->addWidget(mSelect);

    mTranslate = new QPushButton(this);
    mTranslate->setIcon(QIcon(QDir((assetsDir.generic_string() + "translate.png").c_str()).path()));
    mTranslate->setIconSize(QSize(20, 20));
    mTranslate->setCheckable(true);
    this->connect(mTranslate, &QPushButton::toggled, this, &GizmoToolbar::TranslateToggled);
    this->addWidget(mTranslate);

    mRotate = new QPushButton(this);
    mRotate->setIcon(QIcon(QDir((assetsDir.generic_string() + "rotate.png").c_str()).path()));
    mRotate->setIconSize(QSize(20, 20));
    mRotate->setCheckable(true);
    this->connect(mRotate, &QPushButton::toggled, this, &GizmoToolbar::RotateToggled);
    this->addWidget(mRotate);

    mScale = new QPushButton(this);
    mScale->setIcon(QIcon(QDir((assetsDir.generic_string() + "scale.png").c_str()).path()));
    mScale->setIconSize(QSize(20, 20));
    mScale->setCheckable(true);
    this->connect(mScale, &QPushButton::toggled, this, &GizmoToolbar::ScaleToggled);
    this->addWidget(mScale);
  }

  void GizmoToolbar::SelectToggled(bool checked)
  {
    // if this button is now selected
    if (checked)
    {
      // unselect all the other buttons
      mTranslate->setChecked(false);
      mRotate->setChecked(false);
      mScale->setChecked(false);

      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Select);
    }
  }

  void GizmoToolbar::TranslateToggled(bool checked)
  {
    if (checked)
    {
      mSelect->setChecked(false);
      mRotate->setChecked(false);
      mScale->setChecked(false);

      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Translate);
      
    }
  }

  void GizmoToolbar::RotateToggled(bool checked)
  {
    if (checked)
    {
      mSelect->setChecked(false);
      mTranslate->setChecked(false);
      mScale->setChecked(false);

      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Rotate);
    }
  }

  void GizmoToolbar::ScaleToggled(bool checked)
  {
    if (checked)
    {
      mSelect->setChecked(false);
      mTranslate->setChecked(false);
      mRotate->setChecked(false);

      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Scale);
    }
  }

}