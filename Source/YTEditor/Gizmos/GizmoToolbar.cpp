#include <filesystem>

#include <qpushbutton.h>
#include <qdir.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/Gizmos/GizmoButton.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"

namespace YTEditor
{

  GizmoToolbar::GizmoToolbar(MainWindow * aMainWindow) : QToolBar(aMainWindow), mMainWindow(aMainWindow)
  {

    std::experimental::filesystem::path workingDir{ YTE::Path::GetEnginePath().String() };
    std::experimental::filesystem::path assetsDir{ workingDir.append("CreativeCommons_Icons/")};

    QString iconPath = (assetsDir.generic_string() + "select.png").c_str();
    mSelect = new GizmoButton(this, iconPath, &GizmoToolbar::SelectToggled);

    iconPath = (assetsDir.generic_string() + "translate.png").c_str();
    mTranslate = new GizmoButton(this, iconPath, &GizmoToolbar::TranslateToggled);

    iconPath = (assetsDir.generic_string() + "rotate.png").c_str();
    mRotate = new GizmoButton(this, iconPath, &GizmoToolbar::RotateToggled);

    iconPath = (assetsDir.generic_string() + "scale.png").c_str();
    mScale = new GizmoButton(this, iconPath, &GizmoToolbar::ScaleToggled);
    
    this->addWidget(mSelect);
    this->addWidget(mTranslate);
    this->addWidget(mRotate);
    this->addWidget(mScale);

    mButtons.push_back(mSelect);
    mButtons.push_back(mTranslate);
    mButtons.push_back(mRotate);
    mButtons.push_back(mScale);
  }

  std::vector<GizmoButton*>& GizmoToolbar::GetButtons()
  {
    return mButtons;
  }

  void GizmoToolbar::SelectToggled(bool checked)
  {
    // if this button is now selected
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Select);
    }
  }

  void GizmoToolbar::TranslateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Translate);
    }
  }

  void GizmoToolbar::RotateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Rotate);
    }
  }

  void GizmoToolbar::ScaleToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Scale);
    }
  }

}